/*
    Copyright (C) 2008,2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.
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

#ifndef WEBFRAME_H
#define WEBFRAME_H

namespace EA
{
namespace WebKit
{
	class WebFramePrivate;
	class WebPage;
	class WebHitTestResult;
	class WebFrameData;
	class WebHitTestResultPrivate;
	class WebFrame;
}
}

namespace WebCore 
{
	class WidgetPrivate;
	class FrameLoaderClientEA;
	class ChromeClientEA;
	class Node;
    class TextureMapperNodeClientEA;
}


#include <EAWebKit/EAWebKitSystem.h>
#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebkit/EAWebkitView.h>
#include <EAWebkit/EAWebkitJavascriptValue.h>

#include <eastl/vector.h>
#include <eastl/string.h>
#include "KURL.h"
#include "IntRect.h"
#include "IntSize.h"
#include "Color.h"

namespace EA
{
namespace WebKit
{
class WebHitTestResult 
{
public:
	WebHitTestResult();
	WebHitTestResult(const WebHitTestResult &other);
	WebHitTestResult &operator=(const WebHitTestResult &other);
	~WebHitTestResult();

	bool isNull() const;

	bool isContentEditable() const;
	bool isContentSelected() const;


	WebFrame* frame() const;
	WebCore::Node* node() const;
private:
	WebHitTestResult(WebHitTestResultPrivate* priv);
	WebHitTestResultPrivate* d;

	friend class WebFrame;
	friend class WebPagePrivate;
	friend class WebPage;
};

// abaldeva: We write some APIs of this class in terms of WebCore internals. This allows us to be efficient.
// This is okay as we are not planning on exposing this class. We only expose 1 EA::WebKit::View class.
class WebFrame 
{
private:
	WebFrame(WebPage* parent, WebFrameData* frameData);
	WebFrame(WebFrame* parent, WebFrameData* frameData);
	~WebFrame();

public:
	WebPage* page() const;

	void load(const WebCore::KURL& url);
	// abaldeva: The reason to not use WTF::String in the two functions below is that we are already requesting data from 
	// the application in utf8. So converting that data to String and then back to utf8 is simply inefficient. 
	void setHtml(const char8_t* html, size_t length, const WebCore::KURL& baseUrl);
	void setContent(const char8_t* data, size_t length, const char8_t* mediaType, const WebCore::KURL& baseUrl);// example of mediaType or Content-Type is "text/html; charset=utf-8"

	void addToJavaScriptWindowObject(const char8_t *name, IJSBoundObject *obj);
    void GetJavaScriptCallstack(eastl::vector<eastl::string8> *namesOut, eastl::vector<eastl::string8> *argsOut, eastl::vector<int> *linesOut, eastl::vector<eastl::string8> *urlsOut);

	//abaldeva: Could be useful for debugging.
	WTF::String toHtml() const;
	WTF::String toPlainText() const;
	WTF::String renderTreeDump() const;

	WTF::String title() const;

	void setUrl(const WebCore::KURL& url);
	// abaldeva: We'd like to return references below but implementation details of requestedUrl() prohibit us from doing that
	// and we keep the interface uniform.
	WebCore::KURL url() const;
	WebCore::KURL requestedUrl() const;
	WebCore::KURL baseUrl() const;

	WebFrame* parentFrame() const;
	void setParentFrame(WebFrame*); 

	EA::WebKit::ScrollbarPolicy scrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation) const;
	void setScrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation, EA::WebKit::ScrollbarPolicy policy);

	void setScrollBarValue(EA::WebKit::ScrollbarOrientation orientation, int value);
	int scrollBarValue(EA::WebKit::ScrollbarOrientation orientation) const;
	int scrollBarMinimum(EA::WebKit::ScrollbarOrientation orientation) const;
	int scrollBarMaximum(EA::WebKit::ScrollbarOrientation orientation) const;
	WebCore::IntRect scrollBarGeometry(EA::WebKit::ScrollbarOrientation orientation) const;

	void scroll(int, int);
	WebCore::IntPoint scrollPosition() const;
	void setScrollPosition(const WebCore::IntPoint& pos);

	void scrollToAnchor(const WTF::String& anchor);

	enum RenderLayer {
		ContentsLayer = 0x10,
		ScrollBarLayer = 0x20,
		PanIconLayer = 0x40,

		AllLayers = 0xff
	};

    void render(ISurface *surface, const eastl::vector<WebCore::IntRect> &dirtyRegions);
#if USE(ACCELERATED_COMPOSITING)
	void renderAccelerated(const eastl::vector<WebCore::IntRect> &dirtyRegions);
#endif
	void drawHighlightedNodeFromInspector(EA::WebKit::ISurface* surface);
	void setTextSizeMultiplier(float factor);
	float textSizeMultiplier() const;

	float zoomFactor() const;
	void setZoomFactor(float factor);

	bool hasFocus() const;
	void setFocus();

	WebCore::IntPoint pos() const;
	WebCore::IntRect geometry() const;
	WebCore::IntSize contentsSize() const;

	WebHitTestResult hitTestContent(const WebCore::IntPoint& pos, bool excludeScrollbarFromHitTest = true) const;

    bool evaluateJavaScript(const WTF::String& scriptSource, JavascriptValue *resultOut);
    void *GetExec(void);
	enum LoadState 
	{
		kLoadNone,
		kLoadStarted,
		kLoadInProgress,
		kLoadFinished
	};

	LoadState GetLoadState() const;
	void SetLoadState(LoadState loadState);

    void ClearDisplay(const WebCore::Color &color);

private:
	friend class WebPage;
	friend class WebPagePrivate;
	friend class WebFramePrivate;
	friend class WebCore::WidgetPrivate;
	friend class WebCore::FrameLoaderClientEA;
	friend class WebCore::ChromeClientEA;
    friend class WebCore::TextureMapperNodeClientEA;
	WebFramePrivate* d;
};

}
}


#endif //WEBFRAME_H
