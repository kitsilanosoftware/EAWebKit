/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.
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
#include "ContextMenuClientEA.h"

#include "ContextMenu.h"
#include "HitTestResult.h"
#include "KURL.h"
#include "NotImplemented.h"
#include <wtf/RefCounted.h>

#include <stdio.h>

namespace WebCore {
    
void ContextMenuClientEA::contextMenuDestroyed()
{
    delete this;
}

PlatformMenuDescription ContextMenuClientEA::getCustomMenuFromDefaultItems(ContextMenu* menu)
{
#if ENABLE(CONTEXT_MENUS)
    // warning: this transfers the ownership to the caller
    return menu->releasePlatformDescription();
#else
    return PlatformMenuDescription();
#endif
}

void ContextMenuClientEA::contextMenuItemSelected(ContextMenuItem*, const ContextMenu*)
{
    notImplemented();
}

void ContextMenuClientEA::downloadURL(const KURL&)
{
    notImplemented();
}

void ContextMenuClientEA::lookUpInDictionary(Frame*)
{
    notImplemented();
}

void ContextMenuClientEA::speak(const String&)
{
    notImplemented();
}

bool ContextMenuClientEA::isSpeaking()
{
    notImplemented();
    return false;
}

void ContextMenuClientEA::stopSpeaking()
{
    notImplemented();
}

void ContextMenuClientEA::searchWithGoogle(const Frame*)
{
    notImplemented();
}

}

