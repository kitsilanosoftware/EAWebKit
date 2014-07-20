/*
 * Copyright (C) 2007 Staikos Computing Services Inc. <info@staikos.net>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 INdT - Instituto Nokia de Tecnologia
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebPlatformStrategies.h"

#include "Chrome.h"
#include "ChromeClientEA.h"
#include <IntSize.h>
#include "NotImplemented.h"
#include <Page.h>
#include <PageGroup.h>
#include <PluginDatabase.h>
#include <webpage.h>
#include <wtf/MathExtras.h>

#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitLocalizedStringsEnum.h>

using namespace WebCore;

void WebPlatformStrategies::initialize()
{
    DEFINE_STATIC_LOCAL(WebPlatformStrategies, platformStrategies, ());
    (void)platformStrategies;
}

WebPlatformStrategies::WebPlatformStrategies()
{
    setPlatformStrategies(this);
}


CookiesStrategy* WebPlatformStrategies::createCookiesStrategy()
{
    return this;
}

PluginStrategy* WebPlatformStrategies::createPluginStrategy()
{
    return this;
}

LocalizationStrategy* WebPlatformStrategies::createLocalizationStrategy()
{
    return this;
}

VisitedLinkStrategy* WebPlatformStrategies::createVisitedLinkStrategy()
{
    return this;
}

void WebPlatformStrategies::notifyCookiesChanged()
{
}

void WebPlatformStrategies::refreshPlugins()
{
    PluginDatabase::installedPlugins()->refresh();
}

void WebPlatformStrategies::getPluginInfo(const WebCore::Page* page, Vector<WebCore::PluginInfo>& outPlugins)
{
	PluginDatabase* db = PluginDatabase::installedPlugins();
    const Vector<PluginPackage*> &plugins = db->plugins();

    outPlugins.resize(plugins.size());

    for (unsigned int i = 0; i < plugins.size(); ++i) {
        PluginInfo info;
        PluginPackage* package = plugins[i];

        info.name = package->name();
        info.file = package->fileName();
        info.desc = package->description();

        const MIMEToDescriptionsMap& mimeToDescriptions = package->mimeToDescriptions();
        MIMEToDescriptionsMap::const_iterator end = mimeToDescriptions.end();
        for (MIMEToDescriptionsMap::const_iterator it = mimeToDescriptions.begin(); it != end; ++it) {
            MimeClassInfo mime;

            mime.type = it->first;
            mime.desc = it->second;
            mime.extensions = package->mimeToExtensions().get(mime.type);

            info.mimes.append(mime);
        }

        outPlugins.append(info);
    }
}


// LocalizationStrategy
// In the last port, we changed the WebCore layer and used an enum to point to look inside a table like structure. There is a TODO
// note about convincing WebKit folks as it is a better method(saves some code size). However, since we have not done that, a better approach
// would be to fill up following calls. If we want to cut down on the code size later on, we can look into that. 
// I don't expect the overhead to be more than couple of Ks.

//Place Holder text as returning default constructed empty string may crash.
const static char*  missingText = "No Valid String"; 

String getLocalizedString(EA::WebKit::LocalizedStringType type)
{
	using namespace EA::WebKit;

	if(EAWebKitClient* const pClient = GetEAWebKitClient())
	{
		EA::WebKit::LocalizedStringInfo si;
		si.mStringId	= type;
		si.mString16[0] = 0; 
		si.mString8[0]	= 0;
		pClient->GetLocalizedString(si);

		if(si.mString16[0])
			return String(si.mString16);
		else if(si.mString16[8])
			return String::fromUTF8(si.mString8);
	}
	// Move the code inside #if 1 in the demo. We don't want to pay for the default strings.
	const char* p;
#if 1
		switch (type)
		{
			case kLSinputElementAltText:                            p = "Submit"; break;
			case kLSresetButtonDefaultLabel:                        p = "Reset"; break;
			case kLSsearchableIndexIntroduction:                    p = "This is a searchable index. Enter search keywords: "; break;
			case kLSsubmitButtonDefaultLabel:                       p = "Submit"; break;
			case kLSfileButtonChooseFileLabel:                      p = "Choose File"; break;
			case kLSfileButtonNoFileSelectedLabel:                  p = "no file selected"; break;
			case kLScontextMenuItemTagOpenLinkInNewWindow:          p = "Open Link in New Window"; break;
			case kLScontextMenuItemTagDownloadLinkToDisk:           p = "Download Linked File"; break;
			case kLScontextMenuItemTagCopyLinkToClipboard:          p = "Copy Link"; break;
			case kLScontextMenuItemTagOpenImageInNewWindow:         p = "Open Image in New Window"; break;
			case kLScontextMenuItemTagDownloadImageToDisk:          p = "Download Image"; break;
			case kLScontextMenuItemTagCopyImageToClipboard:         p = "Copy Image"; break;
			case kLScontextMenuItemTagOpenFrameInNewWindow:         p = "Open Frame in New Window"; break;
			case kLScontextMenuItemTagCopy:                         p = "Copy"; break;
			case kLScontextMenuItemTagGoBack:                       p = "Back"; break;
			case kLScontextMenuItemTagGoForward:                    p = "Forward"; break;
			case kLScontextMenuItemTagStop:                         p = "Stop"; break;
			case kLScontextMenuItemTagReload:                       p = "Reload"; break;
			case kLScontextMenuItemTagCut:                          p = "Cut"; break;
			case kLScontextMenuItemTagPaste:                        p = "Paste"; break;
			case kLScontextMenuItemTagNoGuessesFound:               p = "No Guesses Found"; break;
			case kLScontextMenuItemTagIgnoreSpelling:               p = "Ignore Spelling"; break;
			case kLScontextMenuItemTagLearnSpelling:                p = "Learn Spelling"; break;
			case kLScontextMenuItemTagSearchWeb:                    p = "Search with Google"; break;
			case kLScontextMenuItemTagLookUpInDictionary:           p = "Look Up in Dictionary"; break;
			case kLScontextMenuItemTagOpenLink:                     p = "Open Link"; break;
			case kLScontextMenuItemTagIgnoreGrammar:                p = "Ignore Grammar"; break;
			case kLScontextMenuItemTagSpellingMenu:                 p = "Spelling and Grammar"; break;
			case kLScontextMenuItemTagShowSpellingMenu:				p = "Show Spelling and Grammar";break;
			case kLScontextMenuItemTagHideSpellingMenu:				p = "Hide Spelling and Grammar";break;
			case kLScontextMenuItemTagCheckSpelling:                p = "Check Document Now"; break;
			case kLScontextMenuItemTagCheckSpellingWhileTyping:     p = "Check Spelling While Typing"; break;
			case kLScontextMenuItemTagCheckGrammarWithSpelling:     p = "Check Grammar With Spelling"; break;
			case kLScontextMenuItemTagFontMenu:                     p = "Font"; break;
			case kLScontextMenuItemTagBold:                         p = "Bold"; break;
			case kLScontextMenuItemTagItalic:                       p = "Italic"; break;
			case kLScontextMenuItemTagUnderline:                    p = "Underline"; break;
			case kLScontextMenuItemTagOutline:                      p = "Outline"; break;
			case kLScontextMenuItemTagWritingDirectionMenu:         p = "Writing Direction"; break;
			case kLScontextMenuItemTagTextDirectionMenu:			p = "Text Direction"; break;
			case kLScontextMenuItemTagDefaultDirection:             p = "Default"; break;
			case kLScontextMenuItemTagLeftToRight:                  p = "Left to Right"; break;
			case kLScontextMenuItemTagRightToLeft:                  p = "Right to Left"; break;
			case kLScontextMenuItemTagOpenVideoInNewWindow:			p = "Open Video in New Window"; break;
			case kLScontextMenuItemTagOpenAudioInNewWindow:			p = "Open Audio in New Window"; break;
			case kLScontextMenuItemTagCopyVideoLinkToClipboard:		p = "Copy Video Link Location"; break;
			case kLScontextMenuItemTagCopyAudioLinkToClipboard:		p = "Copy Audio Link Location"; break;
			case kLScontextMenuItemTagToggleMediaControls:			p = "Toggle Controls"; break;
			case kLScontextMenuItemTagToggleMediaLoop:				p = "Toggle Loop"; break;
			case kLScontextMenuItemTagEnterVideoFullscreen:			p = "Enter Fullscreen"; break;
			case kLScontextMenuItemTagMediaPlay:					p = "Play"; break;
			case kLScontextMenuItemTagMediaPause:					p = "Pause"; break;
			case kLScontextMenuItemTagMediaMute:					p = "Mute"; break;
			case kLScontextMenuItemTagInspectElement:               p = "Inspect Element"; break;
			case kLSsearchMenuNoRecentSearchesText:                 p = "No recent searches"; break;
			case kLSsearchMenuRecentSearchesText:                   p = "Recent Searches"; break;
			case kLSsearchMenuClearRecentSearchesText:              p = "Clear Recent Searches"; break;
			case kLSAXWebAreaText:                                  p = "web area"; break;
			case kLSAXLinkText:                                     p = "link"; break;
			case kLSAXListMarkerText:                               p = "list marker"; break;
			case kLSAXImageMapText:                                 p = "image map"; break;
			case kLSAXHeadingText:                                  p = "heading"; break;
			case kLSAXButtonActionVerb:                             p = "press"; break;
			case kLSAXRadioButtonActionVerb:                        p = "select"; break;
			case kLSAXTextFieldActionVerb:                          p = "activate"; break;
			case kLSAXCheckedCheckBoxActionVerb:                    p = "uncheck"; break;
			case kLSAXUncheckedCheckBoxActionVerb:                  p = "check"; break;
			case kLSAXLinkActionVerb:                               p = "jump"; break;
			case kLSMissingPluginText:								p = "Missing Plug-in";break;
			case kLSunknownFileSizeText:                            p = "Unknown"; break;
			case kLSuploadFileText:                                 p = "Upload file"; break;
			case kLSallFilesText:                                   p = "All Files"; break;
			case kLSMediaElementLoadingStateText:					p = "Loading...";break;
			case kLSMediaElementLiveBroadcastStateText:				p = "Live Broadcast";break;
			case kLSAudioElement:									p = "Audio Element"; break;
			case kLSVideoElement:									p = "Video Element"; break;
			case kLSMuteButton:										p = "Mute Button"; break;
			case kLSUnMuteButton:									p = "Unmute Button"; break;
			case kLSUnPlayButton:									p = "Play Button"; break;
			case kLSUnPauseButton:									p = "Pause Button"; break;
			case kLSSlider:											p = "Slider"; break;
			case kLSSliderThumb:									p = "Slider Thumb"; break;
			case kLSRewindButton:									p = "Rewind Button"; break;
			case kLSReturnToRealTimeButton:							p = "Return to Real-time Button"; break;
			case kLSCurrentTimeDisplay:								p = "Elapsed Time"; break;
			case kLSTimeRemainingDisplay:							p = "Remaining Time"; break;
			case kLSStatusDisplay:									p = "Status Display"; break;
			case KLSFullScreenButton:								p = "Fullscreen Button"; break;
			case KLSSeekForwardButton:								p = "Seek Forward Button"; break;
			case kLSSeekBackButton:									p = "Seek Back Button"; break;
			case kLSAudioElementHelp:								p = "Audio element playback controls and status display"; break;
			case kLSVideoElementHelp:								p = "Video element playback controls and status display"; break;
			case kLSMuteButtonHelp:									p = "Mute audio tracks"; break;
			case kLSUnMuteButtonHelp:								p = "Unmute audio tracks"; break;
			case kLSUnPlayButtonHelp:								p = "Begin playback"; break;
			case kLSUnPauseButtonHelp:								p = "Pause playback"; break;
			case kLSSliderHelp:										p = "Movie time scrubber"; break;
			case kLSSliderThumbHelp:								p = "Movie time scrubber thumb"; break;
			case kLSRewindButtonHelp:								p = "Rewind movie"; break;
			case kLSReturnToRealTimeButtonHelp:						p = "Return streaming movie to real-time"; break;
			case kLSCurrentTimeDisplayHelp:							p = "Current movie time"; break;
			case kLSTimeRemainingDisplayHelp:						p = "Remaining movie time"; break;
			case kLSStatusDisplayHelp:								p = "Current movie status"; break;
			case KLSFullScreenButtonHelp:							p = "Play movie in full-screen mode"; break;
			case KLSSeekForwardButtonHelp:							p = "Seek quickly back"; break;
			case kLSSeekBackButtonHelp:								p = "Seek quickly forward"; break;
			case kLSIndefiniteTime:									p = "Indefinite time";break;
            case kLSFileButtonChooseMultipleFilesLabel:             p = "Choose Files"; break;
            case kLSDefaultDetailsSummaryText:                      p = "Details"; break;
            case kLScontextMenuItemTagCopyImageUrlToClipboard:      p = "Copy Image URL"; break;
			default:                                                p = missingText; break;
		}
#else
		p = missingText;
#endif
		return String::fromUTF8(p);

}

String WebPlatformStrategies::inputElementAltText()
{
	return getLocalizedString(EA::WebKit::kLSinputElementAltText);
}

String WebPlatformStrategies::resetButtonDefaultLabel()
{
	return getLocalizedString(EA::WebKit::kLSresetButtonDefaultLabel);
}

String WebPlatformStrategies::searchableIndexIntroduction()
{
    return getLocalizedString(EA::WebKit::kLSsearchableIndexIntroduction);
}

String WebPlatformStrategies::submitButtonDefaultLabel()
{
    return getLocalizedString(EA::WebKit::kLSsubmitButtonDefaultLabel);
}

String WebPlatformStrategies::fileButtonChooseFileLabel()
{
    return getLocalizedString(EA::WebKit::kLSfileButtonChooseFileLabel);
}

String WebPlatformStrategies::fileButtonChooseMultipleFilesLabel()
{
    return getLocalizedString(EA::WebKit::kLSFileButtonChooseMultipleFilesLabel);
}

String WebPlatformStrategies::fileButtonNoFileSelectedLabel()
{
    return getLocalizedString(EA::WebKit::kLSfileButtonNoFileSelectedLabel);
}

String WebPlatformStrategies::defaultDetailsSummaryText()
{
    return getLocalizedString(EA::WebKit::kLSDefaultDetailsSummaryText);
}


String WebPlatformStrategies::contextMenuItemTagOpenLinkInNewWindow()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenLinkInNewWindow);
}

String WebPlatformStrategies::contextMenuItemTagDownloadLinkToDisk()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagDownloadLinkToDisk);
}

String WebPlatformStrategies::contextMenuItemTagCopyLinkToClipboard()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopyLinkToClipboard);
}

String WebPlatformStrategies::contextMenuItemTagOpenImageInNewWindow()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenImageInNewWindow);
}

String WebPlatformStrategies::contextMenuItemTagDownloadImageToDisk()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagDownloadImageToDisk);
}

String WebPlatformStrategies::contextMenuItemTagCopyImageToClipboard()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopyImageToClipboard);
}

String WebPlatformStrategies::contextMenuItemTagCopyImageUrlToClipboard()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopyImageUrlToClipboard);
}

String WebPlatformStrategies::contextMenuItemTagOpenVideoInNewWindow()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenVideoInNewWindow);
}

String WebPlatformStrategies::contextMenuItemTagOpenAudioInNewWindow()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenAudioInNewWindow);
}

String WebPlatformStrategies::contextMenuItemTagCopyVideoLinkToClipboard()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopyVideoLinkToClipboard);
}

String WebPlatformStrategies::contextMenuItemTagCopyAudioLinkToClipboard()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopyAudioLinkToClipboard);
}

String WebPlatformStrategies::contextMenuItemTagToggleMediaControls()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagToggleMediaControls);
}

String WebPlatformStrategies::contextMenuItemTagToggleMediaLoop()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagToggleMediaLoop);
}

String WebPlatformStrategies::contextMenuItemTagEnterVideoFullscreen()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagEnterVideoFullscreen);
}

String WebPlatformStrategies::contextMenuItemTagMediaPlay()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagMediaPlay);
}

String WebPlatformStrategies::contextMenuItemTagMediaPause()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagMediaPause);
}

String WebPlatformStrategies::contextMenuItemTagMediaMute()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagMediaMute);
}


String WebPlatformStrategies::contextMenuItemTagOpenFrameInNewWindow()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenFrameInNewWindow);
}

String WebPlatformStrategies::contextMenuItemTagCopy()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCopy);
}

String WebPlatformStrategies::contextMenuItemTagGoBack()
{
	return getLocalizedString(EA::WebKit::kLScontextMenuItemTagGoBack);
}

String WebPlatformStrategies::contextMenuItemTagGoForward()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagGoForward);
}

String WebPlatformStrategies::contextMenuItemTagStop()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagStop);
}

String WebPlatformStrategies::contextMenuItemTagReload()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagReload);
}

String WebPlatformStrategies::contextMenuItemTagCut()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCut);
}

String WebPlatformStrategies::contextMenuItemTagPaste()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagPaste);
}

String WebPlatformStrategies::contextMenuItemTagSelectAll()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagSelectAll);
}

String WebPlatformStrategies::contextMenuItemTagNoGuessesFound()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagNoGuessesFound);
}

String WebPlatformStrategies::contextMenuItemTagIgnoreSpelling()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagIgnoreSpelling);
}

String WebPlatformStrategies::contextMenuItemTagLearnSpelling()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagLearnSpelling);
}

String WebPlatformStrategies::contextMenuItemTagSearchWeb()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagSearchWeb);
}

String WebPlatformStrategies::contextMenuItemTagLookUpInDictionary(const String&)
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagLookUpInDictionary);
}

String WebPlatformStrategies::contextMenuItemTagOpenLink()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOpenLink);
}

String WebPlatformStrategies::contextMenuItemTagIgnoreGrammar()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagIgnoreGrammar);
}

String WebPlatformStrategies::contextMenuItemTagSpellingMenu()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagSpellingMenu);
}

String WebPlatformStrategies::contextMenuItemTagShowSpellingPanel(bool show)
{
    return show ? getLocalizedString(EA::WebKit::kLScontextMenuItemTagShowSpellingMenu):
                  getLocalizedString(EA::WebKit::kLScontextMenuItemTagHideSpellingMenu) ;
}

String WebPlatformStrategies::contextMenuItemTagCheckSpelling()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCheckSpelling);
}

String WebPlatformStrategies::contextMenuItemTagCheckSpellingWhileTyping()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCheckSpellingWhileTyping);
}

String WebPlatformStrategies::contextMenuItemTagCheckGrammarWithSpelling()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagCheckGrammarWithSpelling);
}

String WebPlatformStrategies::contextMenuItemTagFontMenu()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagFontMenu);
}

String WebPlatformStrategies::contextMenuItemTagBold()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagBold);
}

String WebPlatformStrategies::contextMenuItemTagItalic()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagItalic);
}

String WebPlatformStrategies::contextMenuItemTagUnderline()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagUnderline);
}

String WebPlatformStrategies::contextMenuItemTagOutline()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagOutline);
}

String WebPlatformStrategies::contextMenuItemTagWritingDirectionMenu()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagWritingDirectionMenu);
}

String WebPlatformStrategies::contextMenuItemTagTextDirectionMenu()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagTextDirectionMenu);
}

String WebPlatformStrategies::contextMenuItemTagDefaultDirection()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagDefaultDirection);
}

String WebPlatformStrategies::contextMenuItemTagLeftToRight()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagLeftToRight);
}

String WebPlatformStrategies::contextMenuItemTagRightToLeft()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagRightToLeft);
}


String WebPlatformStrategies::contextMenuItemTagInspectElement()
{
    return getLocalizedString(EA::WebKit::kLScontextMenuItemTagInspectElement);
}

String WebPlatformStrategies::searchMenuNoRecentSearchesText()
{
    return getLocalizedString(EA::WebKit::kLSsearchMenuNoRecentSearchesText);
}

String WebPlatformStrategies::searchMenuRecentSearchesText()
{
    return getLocalizedString(EA::WebKit::kLSsearchMenuRecentSearchesText);
}

String WebPlatformStrategies::searchMenuClearRecentSearchesText()
{
    return getLocalizedString(EA::WebKit::kLSsearchMenuClearRecentSearchesText);
}

String WebPlatformStrategies::AXWebAreaText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXLinkText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXListMarkerText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXImageMapText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXHeadingText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXDefinitionListTermText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXDefinitionListDefinitionText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXButtonActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXRadioButtonActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXTextFieldActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXCheckedCheckBoxActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXUncheckedCheckBoxActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXMenuListActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXMenuListPopupActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::AXLinkActionVerb()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::missingPluginText()
{
	return getLocalizedString(EA::WebKit::kLSMissingPluginText);
}

String WebPlatformStrategies::crashedPluginText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::multipleFileUploadText(unsigned)
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::unknownFileSizeText()
{
	return getLocalizedString(EA::WebKit::kLSunknownFileSizeText);
}

String WebPlatformStrategies::imageTitle(const String& filename, const IntSize& size)
{
	return String(missingText);
}

String WebPlatformStrategies::mediaElementLoadingStateText()
{
	return getLocalizedString(EA::WebKit::kLSMediaElementLoadingStateText);
}

String WebPlatformStrategies::mediaElementLiveBroadcastStateText()
{
	return getLocalizedString(EA::WebKit::kLSMediaElementLiveBroadcastStateText);
}

#if ENABLE(VIDEO)

String WebPlatformStrategies::localizedMediaControlElementString(const String& name)
{
	if (name == "AudioElement")
		return getLocalizedString(EA::WebKit::kLSAudioElement);
    if (name == "VideoElement")
		return getLocalizedString(EA::WebKit::kLSVideoElement);
    if (name == "MuteButton")
		return getLocalizedString(EA::WebKit::kLSMuteButton);
    if (name == "UnMuteButton")
		return getLocalizedString(EA::WebKit::kLSUnMuteButton);
    if (name == "PlayButton")
		return getLocalizedString(EA::WebKit::kLSUnPlayButton);
    if (name == "PauseButton")
		return getLocalizedString(EA::WebKit::kLSUnPauseButton);
    if (name == "Slider")
		return getLocalizedString(EA::WebKit::kLSSlider);
    if (name == "SliderThumb")
		return getLocalizedString(EA::WebKit::kLSSliderThumb);
    if (name == "RewindButton")
		return getLocalizedString(EA::WebKit::kLSRewindButton);
    if (name == "ReturnToRealtimeButton")
		return getLocalizedString(EA::WebKit::kLSReturnToRealTimeButton);
    if (name == "CurrentTimeDisplay")
		return getLocalizedString(EA::WebKit::kLSCurrentTimeDisplay);
    if (name == "TimeRemainingDisplay")
		return getLocalizedString(EA::WebKit::kLSTimeRemainingDisplay);
    if (name == "StatusDisplay")
		return getLocalizedString(EA::WebKit::kLSStatusDisplay);
    if (name == "FullscreenButton")
		return getLocalizedString(EA::WebKit::KLSFullScreenButton);
    if (name == "SeekForwardButton")
		return getLocalizedString(EA::WebKit::KLSSeekForwardButton);
    if (name == "SeekBackButton")
		return getLocalizedString(EA::WebKit::kLSSeekBackButton);

    return String(missingText);
}

String WebPlatformStrategies::localizedMediaControlElementHelpText(const String& name)
{
	if (name == "AudioElement")
		return getLocalizedString(EA::WebKit::kLSAudioElementHelp);
	if (name == "VideoElement")
		return getLocalizedString(EA::WebKit::kLSVideoElementHelp);
	if (name == "MuteButton")
		return getLocalizedString(EA::WebKit::kLSMuteButtonHelp);
	if (name == "UnMuteButton")
		return getLocalizedString(EA::WebKit::kLSUnMuteButtonHelp);
	if (name == "PlayButton")
		return getLocalizedString(EA::WebKit::kLSUnPlayButtonHelp);
	if (name == "PauseButton")
		return getLocalizedString(EA::WebKit::kLSUnPauseButtonHelp);
	if (name == "Slider")
		return getLocalizedString(EA::WebKit::kLSSliderHelp);
	if (name == "SliderThumb")
		return getLocalizedString(EA::WebKit::kLSSliderThumbHelp);
	if (name == "RewindButton")
		return getLocalizedString(EA::WebKit::kLSRewindButtonHelp);
	if (name == "ReturnToRealtimeButton")
		return getLocalizedString(EA::WebKit::kLSReturnToRealTimeButtonHelp);
	if (name == "CurrentTimeDisplay")
		return getLocalizedString(EA::WebKit::kLSCurrentTimeDisplayHelp);
	if (name == "TimeRemainingDisplay")
		return getLocalizedString(EA::WebKit::kLSTimeRemainingDisplayHelp);
	if (name == "StatusDisplay")
		return getLocalizedString(EA::WebKit::kLSStatusDisplayHelp);
	if (name == "FullscreenButton")
		return getLocalizedString(EA::WebKit::KLSFullScreenButtonHelp);
	if (name == "SeekForwardButton")
		return getLocalizedString(EA::WebKit::KLSSeekForwardButtonHelp);
	if (name == "SeekBackButton")
		return getLocalizedString(EA::WebKit::kLSSeekBackButtonHelp);

	ASSERT_NOT_REACHED();
    return String(missingText);
}

String WebPlatformStrategies::localizedMediaTimeDescription(float time)
{
	if (!isfinite(time))
		return getLocalizedString(EA::WebKit::kLSIndefiniteTime);

    int seconds = (int)fabsf(time);
    int days = seconds / (60 * 60 * 24);
    int hours = seconds / (60 * 60);
    int minutes = (seconds / 60) % 60;
    seconds %= 60;

    if (days)
		return String(missingText);

    if (hours)
		return String(missingText);

    if (minutes)
		return String(missingText);

	return String(missingText);
}

#else // ENABLE(VIDEO)
// FIXME: #if ENABLE(VIDEO) should be in the base class

String WebPlatformStrategies::localizedMediaControlElementString(const String& name)
{
    return String(missingText);
}

String WebPlatformStrategies::localizedMediaControlElementHelpText(const String& name)
{
    return String(missingText);
}

String WebPlatformStrategies::localizedMediaTimeDescription(float time)
{
    return String(missingText);
}

#endif // ENABLE(VIDEO)


String WebPlatformStrategies::validationMessageValueMissingText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessageTypeMismatchText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessagePatternMismatchText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessageTooLongText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessageRangeUnderflowText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessageRangeOverflowText()
{
    notImplemented();
    return String(missingText);
}

String WebPlatformStrategies::validationMessageStepMismatchText()
{
    notImplemented();
    return String(missingText);
}

// VisitedLinkStrategy

bool WebPlatformStrategies::isLinkVisited(Page* page, LinkHash hash)
{
    return page->group().isLinkVisited(hash);
}

void WebPlatformStrategies::addVisitedLink(Page* page, LinkHash hash)
{
    page->group().addVisitedLinkHash(hash);
}
