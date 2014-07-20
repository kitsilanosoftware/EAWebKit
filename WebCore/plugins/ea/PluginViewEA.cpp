/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
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
#include "PluginView.h"
#include "BitmapImage.h"
#include "Bridge.h"
#include "Chrome.h"
#include "ChromeClient.h"
#include "Document.h"
#include "DocumentLoader.h"
#include "Element.h"
#include "EventNames.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoadRequest.h"
#include "FrameLoader.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HTMLNames.h"
#include "HTMLPlugInElement.h"
#include "HostWindow.h"
#include "Image.h"
#include "JSDOMBinding.h"
#include "JSDOMWindow.h"
#include "KeyboardEvent.h"
//#include "LocalWindowsContext.h"
#include "MIMETypeRegistry.h"
#include "MouseEvent.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PluginDatabase.h"
#include "PluginDebug.h"
#include "PluginMainThreadScheduler.h"
#if defined(EA_PLATFORM_WINDOWS)
#include "PluginMessageThrottlerWin.h"
#include "WebCoreInstanceHandle.h"
#endif
#include "PluginPackage.h"
#include "RenderWidget.h"
#include "ScriptController.h"
#include "Settings.h"
#include "c_instance.h"
#include "npruntime_impl.h"
#include "runtime_root.h"
#include <runtime/JSLock.h>
#include <runtime/JSValue.h>
#include <wtf/ASCIICType.h>

namespace WebCore
{
void PluginView::invalidateRect(NPRect*)
{

}

void PluginView::invalidateRegion(NPRegion)
{

}

void PluginView::forceRedraw()
{

}

void PluginView::setNPWindowRect(const IntRect&)
{

}

void PluginView::updatePluginWidget()
{

}

void PluginView::handleKeyboardEvent(KeyboardEvent*)
{

}

void PluginView::handleMouseEvent(MouseEvent*)
{

}

bool PluginView::platformStart()
{
	return false;
}

void PluginView::platformDestroy()
{

}

void PluginView::paint(GraphicsContext*, const IntRect&)
{

}

void PluginView::invalidateRect(const IntRect&)
{

}

void PluginView::setFocus(bool)
{

}

void PluginView::show()
{

}

void PluginView::hide()
{

}

void PluginView::setParentVisible(bool)
{

}

void PluginView::setParent(ScrollView*)
{

}

void PluginView::halt()
{

}

void PluginView::restart()
{

}

NPError PluginView::handlePostReadFile(Vector<char>& buffer, uint32_t len, const char* buf)
{
	return NPERR_NO_ERROR;
}

bool PluginView::platformGetValueStatic(NPNVariable variable, void* value, NPError* result)
{
	return false;
}

bool PluginView::platformGetValue(NPNVariable variable, void* value, NPError* result)
{
	return false;
}
}