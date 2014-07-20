/*
 * Copyright (C) 2000 Peter Kelly (pmk@post.com)
 * Copyright (C) 2005, 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2007 Samuel Weinig (sam@webkit.org)
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "XMLDocumentParser.h"

#include "CDATASection.h"
#include "CachedScript.h"
#include "Comment.h"
#include "CachedResourceLoader.h"
#include "Document.h"
#include "DocumentFragment.h"
#include "DocumentType.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameView.h"
#include "HTMLEntityParser.h"
#include "HTMLHtmlElement.h"
#include "HTMLLinkElement.h"
#include "HTMLNames.h"
#include "HTMLStyleElement.h"
#include "ProcessingInstruction.h"
#include "ResourceError.h"
#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "ScriptableDocumentParser.h"
#include "ScriptElement.h"
#include "ScriptSourceCode.h"
#include "ScriptValue.h"
#include "TextResourceDecoder.h"
#include "TransformSource.h"
#include <wtf/StringExtras.h>
#include <wtf/Threading.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>

#if ENABLE(XHTMLMP)
#include "HTMLNames.h"
#include "HTMLScriptElement.h"
#endif

#include <internal/include/EAWebkitEASTLHelpers.h>
#include <internal/include/EAWebKitAssert.h>

using namespace std;

// Note by Arpit Baldeva: I have verified that all the examples listed here work (and some other complex sites)
// http://www.w3schools.com/xml/xml_examples.asp
// http://www.xml.com/pub/a/2000/03/29/tutorial/index.html


namespace WebCore {

// This is a callback that UTFXml calls when it encounters an entity (e.g. &amp;) that it 
// is not familiar with. This function converts it to a Unicode character (e.g. &).
bool UTFXmlEntityLookup(EA::XML::XmlReader* pReader, const char* pEntityName, void* /*pContext*/)
{
	const UChar c = WebCore::decodeNamedEntity(pEntityName);
	if(c)
	{
		pReader->PushInputBuffer(&c, /*1 * */sizeof(UChar),EA::XML::kReadEncodingUTF16, NULL, true ); 
		return true;
	}

	return false;
}

// --------------------------------

bool XMLDocumentParser::supportsXMLVersion(const String& version)
{
    return version == "1.0";
}

XMLDocumentParser::XMLDocumentParser(Document* document, FrameView* frameView)
    : ScriptableDocumentParser(document)
    , m_view(frameView)
    , m_wroteText(false)
    , m_currentNode(document)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    #if ENABLE(XHTMLMP)
    , m_isXHTMLMPDocument(false)
    , m_hasDocTypeDeclaration(false)
    #endif
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(document)
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition1::belowRangePosition())
    , m_parsingFragment(false)
    , m_scriptingPermission(FragmentScriptingAllowed)
{
	m_stream.SetFlag(0xffffffff, true); 
	m_stream.SetEntityRefResolver(UTFXmlEntityLookup, NULL);
}

XMLDocumentParser::XMLDocumentParser(DocumentFragment* fragment, Element* parentElement, FragmentScriptingPermission permission)
    : ScriptableDocumentParser(fragment->document())
    , m_view(0)
    , m_wroteText(false)
    , m_currentNode(fragment)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    #if ENABLE(XHTMLMP)
    , m_isXHTMLMPDocument(false)
    , m_hasDocTypeDeclaration(false)
    #endif
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(fragment->document())
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition1::belowRangePosition())
    , m_parsingFragment(true)
    , m_scriptingPermission(permission)
{
    fragment->ref();

	m_stream.SetFlag(0xffffffff, true); //Enable all flags.
	m_stream.SetEntityRefResolver(UTFXmlEntityLookup, NULL);
	
	// Add namespaces based on the parent node
    Vector<Element*> elemStack;
    while (parentElement) {
        elemStack.append(parentElement);

        Node* n = parentElement->parentNode();
        if (!n || !n->isElementNode())
            break;
        parentElement = static_cast<Element*>(n);
    }

    if (elemStack.isEmpty())
        return;

	// In the code below, we go through the element hierarchy and figure out extra namespaces that should be added to the reader.
	// The xml namespace could take following form
	// <table xmlns="http://www.w3.org/TR/html4/"> - Here localname = xmlns value = http://www.w3.org/TR/html4/
	// <h:table xmlns:h="http://www.w3.org/TR/html4/"> - Here localname = h and prefix = xmlns and value = http://www.w3.org/TR/html4/
	for (Element* element = elemStack.last(); !elemStack.isEmpty(); elemStack.removeLast()) {
        if (NamedNodeMap* attrs = element->attributes()) {
            for (unsigned i = 0; i < attrs->length(); i++) {
                Attribute* attr = attrs->attributeItem(i);
				const AtomicString& sValue     = attr->value();
				const AtomicString& sLocalName = attr->localName();
				if (attr->localName() == "xmlns")
                    m_defaultNamespaceURI = sValue;
                else if (attr->prefix() == "xmlns")
				{
					EA::WebKit::FixedString8_128 localName8;
					EA::WebKit::ConvertToString8(sLocalName,localName8);

					EA::WebKit::FixedString8_128 value8;
					EA::WebKit::ConvertToString8(sValue,value8);
					m_stream.AddNamespaceAlias(localName8.c_str(), value8.c_str(), true);
				}
            }
        }
    }

	

    // If the parent element is not in document tree, there may be no xmlns attribute; just default to the parent's namespace.
    if (m_defaultNamespaceURI.isNull() && !parentElement->inDocument())
        m_defaultNamespaceURI = parentElement->namespaceURI();

	
}

XMLDocumentParser::~XMLDocumentParser()
{
    clearCurrentNodeStack();
    if (m_pendingScript)
        m_pendingScript->removeClient(this);
}

// Note by Paul Pedriana: The name of this function (doWrite) doesn't mean that
// it's writing a new XML file, it means that more XML data is read to add to 
// the XML input stream to our XmlReader. So it's writing into the XML input buffer.
void XMLDocumentParser::doWrite(const String& parseString)
{
    m_wroteText = true;

    if (document()->decoder() && document()->decoder()->sawError()) {
        // If the decoder saw an error, report it as fatal (stops parsing)
        handleError(XMLErrors::fatal, "Encoding error", lineNumber(), columnNumber());
        return;
    }

	if (parseString.length()) 
	{
		// JavaScript may cause the parser to detach,
		// keep this alive until this function is done.
		RefPtr<XMLDocumentParser> protect(this);

		// Note by Arpit Baldeva: We have a problem here. The UTFXml currently does not support streaming correctly. It it did, the
		// code should look like
#if 0
		m_stream.AppendInputBuffer(parseString.characters(), parseString.length() * sizeof(UChar), EA::XML::kReadEncodingUTF16, NULL, true);
		parse();
#else
		// The workaround code for the current limitation
		// If parsing fragment, we need to call parse() since there is no "doEnd()" call. I have tested and seems like we always have
		// a complete fragment.
		if(m_parsingFragment)
		{
			m_stream.AppendInputBuffer(parseString.characters(), parseString.length() * sizeof(UChar), EA::XML::kReadEncodingUTF16, NULL, true);
			parse();
		}
		else // If we are parsing a document, we simply add the content to the placeholder string and wait for the "doEnd()" to be called.
			// doEnd is called when the server indicates that the data is finished.
		{
			m_docContent.append(parseString.characters(),parseString.length());
		}
#endif
	}
	
}

void XMLDocumentParser::initializeParserContext(const CString&)
{
    DocumentParser::startParsing();
    m_sawError = false;
    m_sawCSS = false;
    m_sawXSLTransform = false;
    m_sawFirstElement = false;
}

void XMLDocumentParser::doEnd()
{
	// abaldeva: Now that complete Xml doc is available, we start parsing it. This won't be required once UTFXml is fixed.
	if(m_docContent.length() && !m_parsingFragment)
	{
		m_stream.AppendInputBuffer(m_docContent.characters(), m_docContent.length() * sizeof(UChar), EA::XML::kReadEncodingUTF16, NULL, true);
		m_docContent.truncate(0); //We make sure that we truncate the length to be zero if we end up being called multiple times.
		parse(); 
	}

#if ENABLE(XSLT)
    if (m_sawXSLTransform) {
        document()->setTransformSource(adoptPtr(new TransformSource(m_originalSourceForTransform)));
        document()->setParsing(false); // Make the doc think it's done, so it will apply xsl sheets.
        document()->styleSelectorChanged(RecalcStyleImmediately);
        document()->setParsing(true);
        DocumentParser::stopParsing();
    }
#endif

	
	if (m_stream.GetResultCode() != EA::XML::XmlReader::kSuccess 
		|| (m_wroteText && !m_sawFirstElement && !m_sawXSLTransform && !m_sawError)) 
	{
        handleError(XMLErrors::fatal, "XML incomplete content error", lineNumber(), columnNumber());
	}
}

// Following two do not seem terribly useful.
int XMLDocumentParser::lineNumber() const
{
    return m_stream.GetLineNumber(); 
}

int XMLDocumentParser::columnNumber() const
{
    return m_stream.GetColumnNumber();
}

TextPosition0 XMLDocumentParser::textPosition() const
{
    return TextPosition0(WTF::ZeroBasedNumber::fromZeroBasedInt(lineNumber()), WTF::ZeroBasedNumber::fromZeroBasedInt(columnNumber()));
}

// This method incorrectly reinterprets zero-base lineNumber method as one-based number.
// FIXME: This error is kept for compatibility. We should fix it eventually. 
TextPosition1 XMLDocumentParser::textPositionOneBased() const
{
    return TextPosition1(WTF::OneBasedNumber::fromOneBasedInt(lineNumber()), WTF::OneBasedNumber::fromOneBasedInt(columnNumber()));
}

void XMLDocumentParser::stopParsing()
{
    ScriptableDocumentParser::stopParsing();
}

void XMLDocumentParser::resumeParsing()
{
    ASSERT(m_parserPaused);

    m_parserPaused = false;

    // First, execute any pending callbacks
    parse();
    if (m_parserPaused)
        return;

    // Then, write any pending data
    SegmentedString rest = m_pendingSrc;
    m_pendingSrc.clear();
    append(rest);

    // Finally, if finish() has been called and append() didn't result
    // in any further callbacks being queued, call end()
    if (m_finishCalled && !m_parserPaused && !m_pendingScript)
        end();
}

bool XMLDocumentParser::appendFragmentSource(const String& source)
{
    ASSERT(!m_sawFirstElement);
	
	// abaldeva: We send it in one shot as we can't parse partial data.
	String fullXml = String("<utfxmlstreamdummyelement>")+source+String("</utfxmlstreamdummyelement>");
    append(fullXml);
	
	//append(String("<utfxmlstreamdummyelement>"));
	//append(source);
	//append(String("</utfxmlstreamdummyelement>"));
    
	return !hasError();
}

// --------------------------------

struct AttributeParseState {
    HashMap<String, String> attributes;
    bool gotAttributes;
};

// Used by parseAttributes below.
static void attributesStartElementNsHandler(AttributeParseState* state, const EA::XML::XmlReader& xmlReader)
{
	int32_t attributeCount = xmlReader.GetAttributeCount();

	if (attributeCount > 0) {
		state->gotAttributes = true;

		for(int32_t i = 0; i < attributeCount; i++) {
			const char* pName  = xmlReader.GetAttributeName(i); //Fully qualified name
			const char* pValue = xmlReader.GetAttributeValue(i);

			String attrQName = UTF8Encoding().decode(pName, strlen(pName));
			String attrValue = UTF8Encoding().decode(pValue, strlen(pValue));

			state->attributes.set(attrQName, attrValue);
		}
	}
}

// This code is executed only for xml-stylesheet tag.
// <?xml-stylesheet type="text/css" href="mystyle.css"?>

HashMap<String, String> parseAttributes(const String& string, bool& attrsOK)
{
	using namespace EA::XML;

	AttributeParseState state;
	state.gotAttributes = false;

	XmlReader    xmlReader;
	const String parseString = "<?xml version=\"1.0\"?><attrs " + string + " />";

	xmlReader.PushInputBuffer(parseString.characters(), parseString.length() * sizeof(UChar), EA::XML::kReadEncodingUTF16, NULL, false);


	while(xmlReader.Read())
	{
		const XmlReader::NodeType nodeType = xmlReader.GetNodeType();

		if (nodeType == XmlReader::Element)
			attributesStartElementNsHandler(&state, xmlReader);
	}

	attrsOK = state.gotAttributes;
	return state.attributes;
}

///////////////////////////////////////////////////////////////////////////////
// We need to handle XML namespaces. The WebCore XML DOM expects elements and
// attributes to be created with WebCore QualifiedNames, where QualifiedName
// is an XML qualified name.
//
// See http://www.w3schools.com/XML/xml_namespaces.asp
// See http://www.w3.org/TR/REC-xml-names/#sec-namespaces
// See http://www.w3.org/TR/REC-xml-names/#ns-qualnames
//
// Example:
//    <h:table xmlns:h="http://www.w3.org/TR/html4/">
//       <h:tr>
//       <h:td>Apples</h:td>
//       <h:td>Bananas</h:td>
//       </h:tr>
//    </h:table>
//    
// The QualifiedName class has the following:
//    AtomicString m_prefix;        // Same as 'h' above
//    AtomicString m_localName;     // Same as 'table' above
//    AtomicString m_namespace;     // Same as 'http://www.w3.org.TR/html4/' above
//
///////////////////////////////////////////////////////////////////////////////

static inline void handleElementNamespaces(WebCore::Element* newElement, const EA::XML::XmlReader& xmlReader, 
										   WebCore::ExceptionCode& ec, FragmentScriptingPermission scriptingPermission)
{
	for (int32_t i = 0, count = xmlReader.GetElementNamespaceCount(); i < count; ++i)
	{
		const char* pNamespaceURI;
		const char* pPrefix = xmlReader.GetElementNamespace(i, &pNamespaceURI);
	
		String sPrefix			= (pPrefix && pPrefix[0]) ? UTF8Encoding().decode(pPrefix, strlen(pPrefix)) : String();
		String sNamespaceQName	= !sPrefix.isEmpty() ? String("xmlns:") + sPrefix : String("xmlns");
		String sNamespaceURI	= UTF8Encoding().decode(pNamespaceURI, strlen(pNamespaceURI));

		newElement->setAttributeNS("http://www.w3.org/2000/xmlns/", sNamespaceQName, sNamespaceURI, ec, scriptingPermission);

		if (ec) // exception setting attributes
		{
			EAW_ASSERT_FORMATTED(false,"Xml parsing exception: %d",ec);
			return;
		}
	}
}

static inline void handleElementAttributes(WebCore::Element* newElement, EA::XML::XmlReader& xmlReader, 
										   WebCore::ExceptionCode& ec, FragmentScriptingPermission scriptingPermission)
{
    for (int32_t i = 0, count = xmlReader.GetAttributeCount(); i < count; ++i)
    {
        const char* pName      = xmlReader.GetAttributeName(i);
        const char* pLocalName = xmlReader.GetAttributeLocalName(i); //It's wrong for this function to just strip off stuff before ':' without verifying if it is actually a namespace prefix.
        const char* pValue     = xmlReader.GetAttributeValue(i);

        String attrLocalName = UTF8Encoding().decode(pLocalName, strlen(pLocalName));
        String attrValue     = UTF8Encoding().decode(pValue, strlen(pValue));

        // Some behavior in WebKit relies on having a string set, even if empty.  
        // This is used for example with input button text which might have been init with a value="" string but if this is set 
        // by UTF8Encoding() as NULL instead of empty, the attribute will fail to be added and some default alt text like "Submit" will be used instead.  
        if (attrValue.isNull()) 
            attrValue = "";

        String attrQName     = UTF8Encoding().decode(pName, strlen(pName));

		// Implement a workaround to get the attribute namespace from UTFXml.
		String attrURI;
		EA::WebKit::FixedString8_32 sPrefix(pName);
		size_t prefixLoc = sPrefix.find(':');
		if( prefixLoc != EA::WebKit::FixedString8_32::npos)
		{
			sPrefix = sPrefix.left(prefixLoc);
			
			const char* pURI = xmlReader.GetNamespaceFromPrefix(pName);
			if(pURI && pURI[0])
				attrURI =  UTF8Encoding().decode(pURI, strlen(pURI));
		
			// Implement a workaround for a bug in WebCore.
			// There could be some rare case when an xml uses ":" in attribute name although the part before ":" does not represent
			// a valid namespace. However, WebCore needs a namespace URI in such cases as it mistakenly thinks one is required.
			// The example is www.xml.com/2000/03/29/tutorial/examples/books2.xml
			
			if(attrURI.isEmpty())
			{
				attrURI = "http://www.w3.org/2000/xmlns/";
			}
		}

        newElement->setAttributeNS(attrURI, attrQName, attrValue, ec, scriptingPermission);

		if (ec) // exception setting attributes
		{
			EAW_ASSERT_FORMATTED(false,"Xml parsing exception: %d",ec);
			return;
		}
    }
}

void XMLDocumentParser::parse()
{
	using namespace EA::XML;

	while (!isStopped() && !m_parserPaused && m_stream.Read()) 
	{
        const XmlReader::NodeType nodeType = m_stream.GetNodeType();
		
		switch (nodeType) 
		{
		case XmlReader::Document:
			{
				startDocument();
				break;
			}

		case XmlReader::Prologue:
			{
				parseProlog();
				break;
			}
		case XmlReader::EndDocument:
			{
				endDocument();
				break;
			}

        case XmlReader::Element: 
			{
#if ENABLE(XHTMLMP)
            if (document()->isXHTMLMPDocument() && !m_hasDocTypeDeclaration) {
                handleError(fatal, "DOCTYPE declaration lost.", lineNumber(), columnNumber());
                break;
            }
#endif
				parseStartElement();
				if(m_stream.IsEmptyElement())
					parseEndElement(); // XmlReader requires us to manually end a standalone element.
				break;
			}
        case XmlReader::EndElement: 
			{
				parseEndElement();
				break;
			}
        case XmlReader::CharacterData: 
			{
				if (m_stream.GetIsCData()) 
					parseCdata();
				else 
					parseCharacters();
				
				break;
			}
        case XmlReader::Comment: 
			{
				parseComment();
				break;
	        }
        case XmlReader::DocTypeDecl: 
			{
				parseDtd();
#if ENABLE(XHTMLMP)
				m_hasDocTypeDeclaration = true;
#endif
				break;
			}
        case XmlReader::EntityRef: 
			{
				if (isXHTMLDocument()
	#if ENABLE(XHTMLMP)
					|| isXHTMLMPDocument()
	#endif
	#if ENABLE(WML)
					|| isWMLDocument()
	#endif
				   ) 
				{
					const UChar c = WebCore::decodeNamedEntity(m_stream.GetName());
					if (!m_leafTextNode)
						enterText();
					
					ExceptionCode ec = 0;
					String str(&c, 1);
					m_leafTextNode->appendData(str, ec);
					if(ec)
					{
						EAW_ASSERT_FORMATTED(false,"Xml parsing exception: %d",ec);
					}
				}
				break;
			}
        case XmlReader::ProcessingInstruction: 
			{
				parseProcessingInstruction();
				break;
			}
		case XmlReader::ElementDecl:
		case XmlReader::AttListDecl:
		case XmlReader::NotationDecl:
			// Ignore these.
			break;
		case XmlReader::None:
		default: 
			{
				const XmlReader::ResultCode result = m_stream.GetResultCode();

				if (result != XmlReader::kSuccess)
                    handleError(XMLErrors::fatal, "XML content error",  m_stream.GetLineNumber(), m_stream.GetColumnNumber());
				
				break;
			}
        }
    }
}

void XMLDocumentParser::startDocument()
{
    initializeParserContext();
}

void XMLDocumentParser::parseProlog()
{
	ExceptionCode ec = 0;

    if (!m_parsingFragment) {
        document()->setXMLStandalone(m_stream.GetIsStandalone(), ec);

		const String version = m_stream.GetDocVersion();
		if (!version.isEmpty())
			document()->setXMLVersion(version, ec);
		
		const String encoding = m_stream.GetDocEncoding();
		if (!encoding.isEmpty())
		     document()->setXMLEncoding(encoding);
    }
}

void XMLDocumentParser::parseStartElement()
{
    if (!m_sawFirstElement && m_parsingFragment) {
        // skip dummy element for fragments
        m_sawFirstElement = true;
        return;
    }

    exitText();

	const char* pName = m_stream.GetLocalName();
	String localName  = UTF8Encoding().decode(pName, strlen(pName));

	const char* pNamespaceURI = m_stream.GetNamespaceURI();
	String uri = pNamespaceURI ? UTF8Encoding().decode(pNamespaceURI, strlen(pNamespaceURI)) : String();
    
	const size_t prefixLength = m_stream.GetNamePrefixLength(); //For some odd reason, GetNamePrefixLength includes the trailing ":". We ignore that below.
	String prefix = prefixLength ? UTF8Encoding().decode(m_stream.GetName(), prefixLength-1) : String();
    
	if (m_parsingFragment && uri.isNull()) {
        ASSERT(prefix.isNull());
        uri = m_defaultNamespaceURI;
    }

	QualifiedName qName(prefix, localName, uri);
    RefPtr<Element> newElement = document()->createElement(qName, true);
    if (!newElement) {
        stopParsing();
        return;
    }

#if ENABLE(XHTMLMP)
    if (!m_sawFirstElement && isXHTMLMPDocument()) {
        // As per 7.1 section of OMA-WAP-XHTMLMP-V1_1-20061020-A.pdf,
        // we should make sure that the root element MUST be 'html' and
        // ensure the name of the default namespace on the root elment 'html'
        // MUST be 'http://www.w3.org/1999/xhtml'
        if (localName != HTMLNames::htmlTag.localName()) {
            handleError(XMLErrors::fatal, "XHTMLMP document expects 'html' as root element.", lineNumber(), columnNumber());
            return;
        }

        if (uri.isNull()) {
            m_defaultNamespaceURI = HTMLNames::xhtmlNamespaceURI;
            uri = m_defaultNamespaceURI;
            m_stream.addExtraNamespaceDeclaration(QXmlStreamNamespaceDeclaration(prefix, HTMLNames::xhtmlNamespaceURI));
        }
    }
#endif

    bool isFirstElement = !m_sawFirstElement;
    m_sawFirstElement = true;

	// WebCore expects that we copy all attributes to the dom element, including 
	// namespace attributes. All elements are expected to have namespaces. But 
	// UTFXml stores namespace attributes and regular attributes for a node 
	// separately, so we get them with the handleElementNamespaces and handleElementAttributes
	// functions.
	ExceptionCode ec = 0;
    handleElementNamespaces(newElement.get(), m_stream, ec, m_scriptingPermission);
    if (ec) {
        stopParsing();
        return;
    }

    handleElementAttributes(newElement.get(), m_stream, ec, m_scriptingPermission);
    if (ec) {
        stopParsing();
        return;
    }

    ScriptElement* scriptElement = toScriptElement(newElement.get());
    if (scriptElement)
        m_scriptStartPosition = textPositionOneBased();

    m_currentNode->parserAddChild(newElement.get());

    pushCurrentNode(newElement.get());
    if (m_view && !newElement->attached())
        newElement->attach();

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    if (newElement->hasTagName(HTMLNames::htmlTag))
        static_cast<HTMLHtmlElement*>(newElement.get())->insertedByParser();
#endif

    if (isFirstElement && document()->frame())
        document()->frame()->loader()->dispatchDocumentElementAvailable();
}

void XMLDocumentParser::parseEndElement()
{
    exitText();

    RefPtr<ContainerNode> n = m_currentNode;
    n->finishParsingChildren();

    if (m_scriptingPermission == FragmentScriptingNotAllowed && n->isElementNode() && toScriptElement(static_cast<Element*>(n.get()))) {
        popCurrentNode();
        ExceptionCode ec;
        n->remove(ec);
        return;
    }

    if (!n->isElementNode() || !m_view) {
        if (!m_currentNodeStack.isEmpty())
            popCurrentNode();
        return;
    }

    Element* element = static_cast<Element*>(n.get());

    // The element's parent may have already been removed from document.
    // Parsing continues in this case, but scripts aren't executed.
    if (!element->inDocument()) {
        popCurrentNode();
        return;
    }

    ScriptElement* scriptElement = toScriptElement(element);
    if (!scriptElement) {
        popCurrentNode();
        return;
    }

    // don't load external scripts for standalone documents (for now)
    ASSERT(!m_pendingScript);
    m_requestingScript = true;

    bool successfullyPrepared = scriptElement->prepareScript(m_scriptStartPosition, ScriptElement::AllowLegacyTypeInTypeAttribute);
    if (!successfullyPrepared) {
#if ENABLE(XHTMLMP)
        if (!scriptElement->isScriptTypeSupported(ScriptElement::AllowLegacyTypeInTypeAttribute))
        document()->setShouldProcessNoscriptElement(true);
#endif
    } else {
        if (scriptElement->readyToBeParserExecuted())
            scriptElement->executeScript(ScriptSourceCode(scriptElement->scriptContent(), document()->url(), m_scriptStartPosition));
        else if (scriptElement->willBeParserExecuted()) {
            m_pendingScript = scriptElement->cachedScript();
                m_scriptElement = element;
                m_pendingScript->addClient(this);

            // m_pendingScript will be 0 if script was already loaded and addClient() executed it.
                if (m_pendingScript)
                    pauseParsing();
            } else
                m_scriptElement = 0;
    }
    m_requestingScript = false;
    popCurrentNode();
}

void XMLDocumentParser::parseCharacters()
{
    if (!m_leafTextNode)
        enterText();
    
	ExceptionCode ec = 0;
	const char* pText = m_stream.GetValue();
	String sText = UTF8Encoding().decode(pText, m_stream.GetValueLength());//Instead of strlen(pText)), we use GetValueLength as pText may contain embedded NULLs(Text tokens may have embedded NUL characters).
	m_leafTextNode->appendData(sText, ec);
}

void XMLDocumentParser::parseProcessingInstruction()
{
    exitText();

    // ### handle exceptions
    int exception = 0;
    
	const char* pName = m_stream.GetName();
	String sName = UTF8Encoding().decode(pName, strlen(pName));

	const char* pValue = m_stream.GetValue(); // The documentation says that it will return target here which is wrong. It returns data (which is correct behavior anyway so documentation is wrong).
	String sValue = UTF8Encoding().decode(pValue, strlen(pValue));
	
	RefPtr<ProcessingInstruction> pi = document()->createProcessingInstruction(sName, sValue, exception);
    if (exception)
        return;

    pi->setCreatedByParser(true);

    m_currentNode->parserAddChild(pi.get());
    if (m_view && !pi->attached())
        pi->attach();

    pi->finishParsingChildren();

    if (pi->isCSS())
        m_sawCSS = true;
#if ENABLE(XSLT)
    m_sawXSLTransform = !m_sawFirstElement && pi->isXSL();
    if (m_sawXSLTransform && !document()->transformSourceDocument())
        stopParsing();
#endif
}

void XMLDocumentParser::parseCdata()
{
    exitText();

	const char* pText = m_stream.GetValue();
	String sText = UTF8Encoding().decode(pText, strlen(pText));
	
	RefPtr<CDATASection> newNode = CDATASection::create(document(), sText);

    m_currentNode->parserAddChild(newNode.get());
    if (m_view && !newNode->attached())
        newNode->attach();
}

void XMLDocumentParser::parseComment()
{
    exitText();

	const char* pText = m_stream.GetValue();
	String sText = UTF8Encoding().decode(pText, strlen(pText));
	
	RefPtr<Comment> newNode = Comment::create(document(), sText);

    m_currentNode->parserAddChild(newNode.get());
    if (m_view && !newNode->attached())
        newNode->attach();
}

void XMLDocumentParser::endDocument()
{
#if ENABLE(XHTMLMP)
    m_hasDocTypeDeclaration = false;
#endif
}

bool XMLDocumentParser::hasError() const
{
    return m_stream.GetResultCode() != EA::XML::XmlReader::kSuccess;
}

void XMLDocumentParser::parseDtd()
{
	const char* name     = m_stream.GetDocTypeName();
	const char* publicId = m_stream.GetPublicId();
	const char* systemId = m_stream.GetSystemId();

	if(publicId && strstr(publicId, "DTD XHTML")) // e.g. "-//W3C//DTD XHTML 1.0 Transitional//EN"
		setIsXHTMLDocument(true); // controls if we replace entities or not.

#if ENABLE(XHTMLMP)
    else if ((publicId == QLatin1String("-//WAPFORUM//DTD XHTML Mobile 1.1//EN"))
             || (publicId == QLatin1String("-//WAPFORUM//DTD XHTML Mobile 1.0//EN"))) {
        if (AtomicString(name) != HTMLNames::htmlTag.localName()) {
            handleError(fatal, "Invalid DOCTYPE declaration, expected 'html' as root element.", lineNumber(), columnNumber());
            return;
        }

        if (document()->isXHTMLMPDocument()) // check if the MIME type is correct with this method
            setIsXHTMLMPDocument(true);
        else
            setIsXHTMLDocument(true);
    }
#endif
#if ENABLE(WML)
    else if (document()->isWMLDocument()
             && publicId != QLatin1String("-//WAPFORUM//DTD WML 1.3//EN")
             && publicId != QLatin1String("-//WAPFORUM//DTD WML 1.2//EN")
             && publicId != QLatin1String("-//WAPFORUM//DTD WML 1.1//EN")
             && publicId != QLatin1String("-//WAPFORUM//DTD WML 1.0//EN"))
        handleError(fatal, "Invalid DTD Public ID", lineNumber(), columnNumber());
#endif
    if (!m_parsingFragment)
	{
		String sName     = (name		? UTF8Encoding().decode(name,     strlen(name))		: String());
		String sPublicId = (publicId	? UTF8Encoding().decode(publicId, strlen(publicId)) : String());
		String sSystemId = (systemId	? UTF8Encoding().decode(systemId, strlen(systemId)) : String());

		document()->parserAddChild(DocumentType::create(document(), sName, sPublicId, sSystemId));
	}
}
}
