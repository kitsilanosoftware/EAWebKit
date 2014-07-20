/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "Pasteboard.h"

#include "DocumentFragment.h"
#include "Editor.h"
#include "Frame.h"
#include "Image.h"
#include "markup.h"
#include "RenderImage.h"
#include <WebFrame.h>
#include <WebFrame_p.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
namespace WebCore {

Pasteboard::Pasteboard()
    : m_selectionMode(false)
{
}

Pasteboard* Pasteboard::generalPasteboard()
{
    static Pasteboard* pasteboard = 0;
    if (!pasteboard)
        pasteboard = new Pasteboard();
    return pasteboard;
}

void Pasteboard::writeSelection(Range* /*selectedRange*/, bool /*canSmartCopyOrDelete*/, Frame* frame)
{
    String text = frame->editor()->selectedText();
    return writePlainText(text);
}

bool Pasteboard::canSmartReplace()
{
    return false;
}

String Pasteboard::plainText(Frame* pFrame)
{
    using namespace EA::WebKit;
	EA::WebKit::WebFrame* pWebFrame = EA::WebKit::WebFramePrivate::kit(pFrame);
	EA::WebKit::View* pEAWebKitView = pWebFrame->page()->view();
	
	if(EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(pEAWebKitView)) 
    {
        ClipboardEventInfo cei;
        cei.mpView = pEAWebKitView;
        cei.mpUserData = pEAWebKitView->GetUserData();
        cei.mType = kClipBoardEventReadPlainText;
        pClient->ClipboardEvent(cei); 
        if(!GetFixedString(cei.mText)->empty())
            return String(GetFixedString(cei.mText)->c_str());
    }
    return String();
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    return 0;
}

void Pasteboard::writePlainText(const String& text)
{
    using namespace EA::WebKit;
    if(EAWebKitClient* const pClient = GetEAWebKitClient()) 
    {
        ClipboardEventInfo cei;
        cei.mpView = NULL;      // We don't have any view info at this level.
        cei.mpUserData = NULL;  // Since we have no view, no user data.
        cei.mType = kClipBoardEventWritePlainText;
        GetFixedString(cei.mText)->assign(text.characters(), text.length());
        pClient->ClipboardEvent(cei); 
    }
}

void Pasteboard::writeURL(const KURL& _url, const String&, Frame*)
{
    ASSERT(!_url.isEmpty());
}

void Pasteboard::writeImage(Node* node, const KURL&, const String&)
{
    ASSERT(node && node->renderer() && node->renderer()->isImage());
}

/* This function is called from Editor::tryDHTMLCopy before actually set the clipboard
 * It introduce a race condition with klipper, which will try to grab the clipboard 
 * It's not required to clear it anyway, since QClipboard take care about replacing the clipboard
 */
void Pasteboard::clear()
{
}

bool Pasteboard::isSelectionMode() const
{
    return m_selectionMode;
}

void Pasteboard::setSelectionMode(bool selectionMode)
{
    m_selectionMode = selectionMode;
}

}
