/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.
	Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.

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

#ifndef WEBFRAME_P_H
#define WEBFRAME_P_H

#include "webframe.h"
#include "webpage_p.h"

#include "EventHandler.h"
#include "GraphicsContext.h"
#include "KURL.h"
#include "PlatformString.h"
#include "wtf/RefPtr.h"
#include "RefPtrCairo.h"
#include "Frame.h"
#include "ViewportArguments.h"

#if USE(ACCELERATED_COMPOSITING)
#include "texmap/TextureMapper.h"
#include "texmap/TextureMapperNode.h"
#include <EAWebkit/EAWebKitHardwareRenderer.h>
#endif

namespace WebCore 
{
    class FrameLoaderClientEA;
    class FrameView;
    class HTMLFrameOwnerElement;
    class Scrollbar;
    class TextureMapperContentLayer;

}

namespace EA
{
namespace WebKit
{

class WebPage;

class WebFrameData 
{
public:
	WebFrameData(WebCore::Page*, WebCore::Frame* parentFrame = 0,
		WebCore::HTMLFrameOwnerElement* = 0,
		const WTF::String& frameName = WTF::String());

	WebCore::KURL url;
	WTF::String name;
	WebCore::HTMLFrameOwnerElement* ownerElement;
	WebCore::Page* page;
	RefPtr<WebCore::Frame> frame;
	WebCore::FrameLoaderClientEA* frameLoaderClient;

	WTF::String referrer;
	bool allowsScrolling;
	int marginWidth;
	int marginHeight;
};

class WebFramePrivate 
{
public:
	WebFramePrivate()
		: webFrame(0)
		, parentWebFrame(0)
		, horizontalScrollBarPolicy(EA::WebKit::ScrollbarAuto)
		, verticalScrollBarPolicy(EA::WebKit::ScrollbarAuto)
		, frameLoaderClient(0)
		, frame(0)
		, page(0)
		, allowsScrolling(true)
		, marginWidth(-1)
		, marginHeight(-1)
		, mLoadState(WebFrame::kLoadNone)
#if USE(ACCELERATED_COMPOSITING)
        , rootTextureMapperNode(0)
#endif
        , mClearDisplaySurface(false)
        , mHorizontalScroll(NULL)
        , mVerticalScroll(NULL)
        , mScrollCorner(NULL)
	{
		dummyCairoSurface = adoptRef(cairo_image_surface_create_for_data(NULL, CAIRO_FORMAT_ARGB32, 0, 0, 0));    
		dummyCairoContext = adoptRef(cairo_create(dummyCairoSurface.get()));
		dummyGraphicsContext = adoptPtr(new WebCore::GraphicsContext(dummyCairoContext.get()));
	}
	void init(WebFrame* frame, WebFrameData* frameData);
	void setPage(WebPage*);

	inline WebFrame* parentFrame() 
	{ 
		return parentWebFrame;  
	}

	inline void setParentFrame(WebFrame* wFrame)
	{
		parentWebFrame = wFrame;
	}

    WebCore::Scrollbar* horizontalScrollBar() const;
    WebCore::Scrollbar* verticalScrollBar() const;

	static WebCore::Frame* core(const WebFrame*);
	static WebFrame* kit(const WebCore::Frame*);

	void urlChanged();

	WebFrame* webFrame;
	WebFrame* parentWebFrame;
	EA::WebKit::ScrollbarPolicy horizontalScrollBarPolicy;
	EA::WebKit::ScrollbarPolicy verticalScrollBarPolicy;
	WebCore::FrameLoaderClientEA *frameLoaderClient;
	WebCore::Frame *frame;
	WebPage* page;
    WebCore::KURL url;

    bool allowsScrolling;
    int marginWidth;
    int marginHeight;
	WebFrame::LoadState mLoadState;
#if USE(ACCELERATED_COMPOSITING)
	RefPtr<cairo_surface_t> dummyCairoSurface;    
	RefPtr<cairo_t> dummyCairoContext;
	OwnPtr<WebCore::GraphicsContext> dummyGraphicsContext;
	void renderCompositedLayers(WebCore::GraphicsContext* context, const eastl::vector<WebCore::IntRect> &dirtyRegions);
    void renderScrollbar(WebCore::Scrollbar *bar, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions);
    void renderScrollCorner(WebCore::FrameView *view, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions);

    WebCore::TextureMapperNode* rootTextureMapperNode;
    OwnPtr<WebCore::TextureMapper> textureMapper;
#endif

    bool mClearDisplaySurface;
    WebCore::Color mClearDisplayColor;

    ISurface *mHorizontalScroll;
    ISurface *mVerticalScroll;
    ISurface *mScrollCorner;
};

class WebHitTestResultPrivate {
public:
	WebHitTestResultPrivate() : isContentEditable(false), isContentSelected(false), isScrollBar(false) {}
	WebHitTestResultPrivate(const WebCore::HitTestResult &hitTest);

	bool isContentEditable;
	bool isContentSelected;
	bool isScrollBar;
	WebFrame* frame;
	RefPtr<WebCore::Node> innerNode;
	RefPtr<WebCore::Node> innerNonSharedNode;
};

}
}

#endif//WEBFRAME_P_H
