/*
    Copyright (C) 2008 Holger Hans Peter Freyther
    Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
	Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "config.h"
#include "FontPlatformData.h"
#include "PlatformString.h"
#include "SharedBuffer.h"

#include <float.h>
#include <math.h>

#include <EAWebKit/EAWebKit.h>          
#include <internal/include/EAWebKitString.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <EASTL/algorithm.h>
#include <internal/include/EAWebkit_p.h>

namespace WebCore {


FontPlatformDataPrivate::~FontPlatformDataPrivate()
{
	if(mpFont)
	{
		mpFont->Release();
		mpFont = NULL;
	}
}
float GetFontAdjustedSize(float fontSize)
{
    
    // Set TextStyle::mfSize
    //
    // Do we need/want to do this 72/96 dpi transformation? Fonts seem to big relative to 
    // FireFox/Safari without it. The implication is that WebCore expects that the video 
    // display is 72 point instead of 96 point (the latter of which is standard on Windows).
    // Update:09/01/2011 - abaldeva. Yes, this conversion is correct. The Qt port passes down the size as the Pixel size as
	// opposed to Point size. However, it seems like this conversion should use a variable constant based rather than fixed 
	// factor of 0.75f. 
	// 
    // Here is a table of HTML point sizes to WebKit's resulting pixel sizes.
    // Recall that you can specify text in HTML as pixels and points (and some other less common types).
    // The pixel size is always the point size * 1.33333
    //      HTML point size  | computedSize
    //      --------------------------------
    //             9         |    12.000
    //            10         |    13.333
    //            11         |    14.666
    //            12         |    16.000
    //            etc.       |     etc.
    //
    // The following calculation has been shown to perfectly match the font usage of 
    // FireFox and Internet Explorer on Windows XP.
      
    float adjustedSize = fontSize * 0.75f;  // (72.0 / 96.0 = 0.75)
   
    if(adjustedSize <= 0.0f)
    {
        adjustedSize = 1.0f;    // We need some kind of size for EAText. WebKit considers any size over >=0 as valid.
    }
    return adjustedSize;
}

float GetFontAdjustedWeight(FontWeight weight)
{
    COMPILE_ASSERT( FontWeight100 == 0, FontWeight);
    COMPILE_ASSERT( FontWeight900 == 8, FontWeight);

    //const WebCore::FontWeight fontWeight = fontDescription.weight();
    //pTextStyle->SetWeight((fontWeight + 1) * 100);
    
    
    unsigned int w = ((weight + 1) * 100);
    return static_cast<float> (w);
}

static bool IsFontSmooth(float fontSize, FontSmoothingMode mode = AutoSmoothing)
{
    bool isSmooth = false;

    switch (mode)
    {
    case NoSmoothing:
        // False is already set.
        break;
    
    case SubpixelAntialiased:
    case Antialiased:
        isSmooth = true;
        break;
   
    case AutoSmoothing:
    default:
        {
            const uint32_t smoothSize = EA::WebKit::GetParameters().mSmoothFontSize;    
            if ((smoothSize == 0) || (( uint32_t) fontSize >= smoothSize)) // 0 will make all fonts smooth regardless of size.  
            {
                isSmooth = true;
            }
        }
        break;
    }
  
    return isSmooth;
}

// Helper default init for text style.  We could move this to wrapper if needed but it is EAWebKit specific.
static void InitTextStyle(EA::WebKit::TextStyle& textStyle)
{
    textStyle.mfSize            = 0.f;
    textStyle.mStyle            = EA::WebKit::kStyleNormal;
    textStyle.mfWeight          = EA::WebKit::kWeightNormal;
    textStyle.mPitch            = EA::WebKit::kPitchVariable;
    textStyle.mVariant          = EA::WebKit::kVariantNormal;
    textStyle.mSmooth           = EA::WebKit::kSmoothNone;
    textStyle.mFamilyNameArray[0][0] = 0; 
}

// pDest is Null-terminated.
static void CopyFontFamilyName(char16_t* pDest, const WTF::AtomicString& name)
{
	size_t numCharsToCopy = eastl::min_alt<size_t>(name.length(), EA::WebKit::kFamilyNameCapacity - 1);
	EAW_ASSERT_MSG(numCharsToCopy < EA::WebKit::kFamilyNameCapacity, "Font family name too long. Font is not going to be detected properly");
	EA::Internal::Strncpy(pDest, name.characters(),numCharsToCopy);
	pDest[numCharsToCopy] = 0;
}

static inline bool isEmtpyValue(const float size, const bool bold, const bool oblique)
{
     // this is the empty value by definition of the trait FontDataCacheKeyTraits
    return !bold && !oblique && size == 0.f;
}

FontPlatformData::FontPlatformData(float size, bool bold, bool oblique)
{
    if (isEmtpyValue(size, bold, oblique))
        m_data = 0;
    else
        m_data = new FontPlatformDataPrivate(size, bold, oblique);
}

FontPlatformData::FontPlatformData(const FontPlatformData &other) : m_data(other.m_data)
{
    if (m_data && m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1))
        ++m_data->refCount;
}

FontPlatformData::FontPlatformData(const FontDescription& description, const AtomicString& familyName, const UChar* characters, int length, int wordSpacing, int letterSpacing)
    : m_data(new FontPlatformDataPrivate())
{
    using namespace EA::WebKit;

    EA::WebKit::TextStyle textStyle;  // We rather use the textStyle so we can pass in the spacing and the fx info directly. 
    InitTextStyle(textStyle);
    
	// Note by Arpit Baldeva:
	// The way WebCore expects us to handle creation of FontPlatformData is to take the familyName
	// in FontCacheEA.cpp and return a valid pointer only if that particular font can be created.
	// In CSS, one can specify multiple font families in order of preference. If you return NULL,
	// WebCore sends the next highest priority font. Note that it also converts generic font family
	// name (such as serif) to the one you specify as your default in the Settings (For example,
	// Times New Roman for us). So the system is supposed to be pretty simple.
	
	// The problem we have is that EAText wants to builds a priority array in one shot. Also, if you
	// request a font and it is not found, it may return one of the fallback fonts even though you may
	// have something down the priority order available in EAText.
	
	// So what we do here is build up that array.
	
	//-- Build FamilyName array ---
	ITextSystem* pTextSystem = GetTextSystem();
	const uint32_t familyNameArrayCapacity = pTextSystem->GetFamilyNameArrayCapacity();

	uint32_t i = 0;
	EAW_ASSERT_MSG(!familyName.isEmpty(), "Family name can never be empty");
	CopyFontFamilyName(textStyle.mFamilyNameArray[i],familyName);
    
	const FontFamily* pfontFamily = &(description.family());
	pfontFamily = pfontFamily->next();
	++i;

	const EA::WebKit::Parameters& params = EA::WebKit::GetParameters();
	
	// Here, we iterate through the list and copy the fonts to the destination array in the order of priority.
	// If we come across a generic font, we read it from the Params based on the generic family set in the description. 
	bool genericFontAdded = false;
	while(pfontFamily && i < familyNameArrayCapacity)
	{
		if(pfontFamily->family().startsWith("-webkit",true)) // A generic font family 
		{
			genericFontAdded = true;
			const char16_t* type = 0;
			switch(description.genericFamily())
			{
			case FontDescription::SerifFamily:
				type = params.mFontFamilySerif;
				break;

			case FontDescription::SansSerifFamily:
				type = params.mFontFamilySansSerif;
				break;

			case FontDescription::MonospaceFamily:
				type = params.mFontFamilyMonospace;
				break;

			case FontDescription::CursiveFamily:
				type = params.mFontFamilyCursive;
				break;

			case FontDescription::FantasyFamily:
				type = params.mFontFamilyFantasy;
				break;

			default:
			case FontDescription::NoFamily:
			case FontDescription::StandardFamily:
				type = params.mFontFamilyStandard;
				break;
			}

			if(type)
			{
				EA::Internal::Strcpy(textStyle.mFamilyNameArray[i],type);
				++i;
			}
			break;
		}

		CopyFontFamilyName(textStyle.mFamilyNameArray[i], pfontFamily->family());
		++i;
		pfontFamily = pfontFamily->next();
	}
	
	// If we went through all the fonts specified but a generic font was not added, we add the standard font as a fallback.
	// It is probably not a good practice to not specify a generic font family for any font but we deal with that situation here.
	if( i < familyNameArrayCapacity && !genericFontAdded)
	{
		EA::Internal::Strcpy(textStyle.mFamilyNameArray[i],params.mFontFamilyStandard);
		++i;
	}

	if(i < familyNameArrayCapacity)
		*textStyle.mFamilyNameArray[i] = 0;

	// verify that spacing can be used raw without size adjustments. 
    textStyle.mfLetterSpacing =  static_cast<float> (letterSpacing);   
    textStyle.mfWordSpacing =  static_cast<float> (wordSpacing);   

     // Size
    const float fFontSize = description.computedSize();
    const float fAdjustedFontSize = GetFontAdjustedSize(fFontSize); 
    textStyle.mfSize = fAdjustedFontSize;   
    
    // Italic
    const bool bItalic = description.italic();
    if(bItalic)
        textStyle.mStyle = EA::WebKit::kStyleItalic; 
      
    // Weight
    const WebCore::FontWeight fontWeight = description.weight();
    textStyle.mfWeight = GetFontAdjustedWeight(fontWeight);
    
    // Variant
    if(description.smallCaps())
        textStyle.mVariant = EA::WebKit::kVariantSmallCaps;

    // Smooth
    // We act like FireFox under Windows does with respect to sizes and weights.
    bool smooth = IsFontSmooth(fFontSize, description.fontSmoothing());
    if(smooth)
        textStyle.mSmooth = EA::WebKit::kSmoothEnabled;    

    // Now get the requested font 
    if(pTextSystem)      
    { 
        IFont* pFont = pTextSystem->GetFont(textStyle, length ? characters[0] : ' ');
        EAW_ASSERT(pFont);
        m_data->mpFont = pFont;
    }
}

FontPlatformData::FontPlatformData(SharedBuffer* buffer, int size, bool bold, bool italic)
   : m_data(new FontPlatformDataPrivate())   
{
	m_data->size = GetFontAdjustedSize(size);
    m_data->bold = bold;
	m_data->oblique = italic;
	
    // Seems like if the data is "not correct", EAText would crash. For example, try "abc",3.
	m_data->mpFont = EA::WebKit::GetTextSystem()->CreateNewFont(EA::WebKit::kFontTypeOutline, buffer->data(),(uint32_t)buffer->size());
	m_data->mpFont->SetSize(m_data->size);

    // Set text smoothing. 
    bool useSmooth = IsFontSmooth(m_data->size);
    EA::WebKit::Smooth smooth = useSmooth ? EA::WebKit::kSmoothEnabled : EA::WebKit::kSmoothNone;
    m_data->mpFont->SetSmoothing(smooth);

    // We are currently unable to set bold and italic info. Figure out a way for it. 

}

FontPlatformData::~FontPlatformData()
{
    if (!m_data || m_data == reinterpret_cast<FontPlatformDataPrivate*>(-1))
        return;
    --m_data->refCount;
    if (!m_data->refCount)
        delete m_data;
}

FontPlatformData& FontPlatformData::operator=(const FontPlatformData& other)
{
    if (m_data == other.m_data)
        return *this;
    if (m_data && m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1)) {
        --m_data->refCount;
        if (!m_data->refCount)
            delete m_data;
    }
    m_data = other.m_data;
    if (m_data && m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1))
        ++m_data->refCount;
    return *this;
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_data == other.m_data)
        return true;

    if (!m_data || !other.m_data
        || m_data == reinterpret_cast<FontPlatformDataPrivate*>(-1) || other.m_data == reinterpret_cast<FontPlatformDataPrivate*>(-1))
        return  false;

    const bool equals = ((fabs(m_data->size - other.m_data->size) < FLT_EPSILON) 
                         && m_data->bold == other.m_data->bold
                         && m_data->oblique == other.m_data->oblique
                         && m_data->mpFont == other.m_data->mpFont);
    return equals;
}

// Note by Paul Pedriana: it is not clear to me that our hash() must use the 
// StringImpl::computeHash function or if we can use an arbitrary one.
// In any case, the StringImpl::computeHash function is more complex than 
// it needs to be for most uses and could stand to be simplified.
unsigned FontPlatformData::hash() const
{
    const uintptr_t hashCode = reinterpret_cast<uintptr_t>(m_data->mpFont);
    return  StringHasher::computeHash(reinterpret_cast<const UChar*>(&hashCode), sizeof(hashCode) / sizeof(UChar));
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
    return String();
}
#endif

}
