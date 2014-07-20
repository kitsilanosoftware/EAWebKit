/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#include "ClipboardEA.h"

#include "CachedImage.h"
#include "Document.h"
#include "DragData.h"
#include "Element.h"
#include "FileList.h"
#include "Frame.h"
#include "HTMLNames.h"
#include "HTMLParserIdioms.h"
#include "Image.h"
#include "IntPoint.h"
#include "KURL.h"
#include "NotImplemented.h"
#include "PlatformString.h"
#include "Range.h"
#include "RenderImage.h"
#include "markup.h"
#include <wtf/text/StringHash.h>

namespace WebCore {
/*
static bool isTextMimeType(const String& type)
{
    return type == "text/plain" || type.startsWith("text/plain;");
}

static bool isHtmlMimeType(const String& type)
{
    return type == "text/html" || type.startsWith("text/html;");
}
*/
PassRefPtr<Clipboard> Clipboard::create(ClipboardAccessPolicy policy, DragData* dragData, Frame*)
{
    return ClipboardEA::create(policy);//, dragData->platformData());
}
ClipboardEA::ClipboardEA(ClipboardAccessPolicy policy, ClipboardType clipboardType)
    : Clipboard(policy, clipboardType)
{
    ASSERT(policy == ClipboardReadable || policy == ClipboardWritable || policy == ClipboardNumb);

}

ClipboardEA::~ClipboardEA()
{

}

void ClipboardEA::clearData(const String& type)
{
	if (policy() != ClipboardWritable)
        return;
}

void ClipboardEA::clearAllData()
{
	if (policy() != ClipboardWritable)
        return;
}

String ClipboardEA::getData(const String& type, bool& success) const
{
	return String();
}

bool ClipboardEA::setData(const String& type, const String& data)
{
	return false;
}

// extensions beyond IE's API
HashSet<String> ClipboardEA::types() const
{
	return HashSet<String>();
}

PassRefPtr<FileList> ClipboardEA::files() const
{
    notImplemented();
    return 0;
}

void ClipboardEA::setDragImage(CachedImage* image, const IntPoint& point)
{
    setDragImage(image, 0, point);
}

void ClipboardEA::setDragImageElement(Node* node, const IntPoint& point)
{
    setDragImage(0, node, point);
}

void ClipboardEA::setDragImage(CachedImage* image, Node *node, const IntPoint &loc)
{

}

DragImageRef ClipboardEA::createDragImage(IntPoint& dragLoc) const
{
   return NULL;
}

/*
static CachedImage* getCachedImage(Element* element)
{
    // Attempt to pull CachedImage from element
    ASSERT(element);
    RenderObject* renderer = element->renderer();
    if (!renderer || !renderer->isImage())
        return 0;

    RenderImage* image = toRenderImage(renderer);
    if (image->cachedImage() && !image->cachedImage()->errorOccurred())
        return image->cachedImage();

    return 0;
}
*/
void ClipboardEA::declareAndWriteDragImage(Element* element, const KURL& url, const String& title, Frame* frame)
{

}

void ClipboardEA::writeURL(const KURL& url, const String& title, Frame* frame)
{

}

void ClipboardEA::writeRange(Range* range, Frame* frame)
{

}

void ClipboardEA::writePlainText(const String& str)
{

}

bool ClipboardEA::hasData()
{
	return false;
}

}
