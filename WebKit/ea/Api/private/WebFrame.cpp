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

#include "config.h"
#include "WebFrame.h"

#if USE(JSC)
#include "APICast.h"
#include "BridgeJSC.h"
#include "CallFrame.h"
#elif USE(V8)
#include "V8Binding.h"
#endif
#include "Document.h"
#include "DocumentLoader.h"
#include "DragData.h"
#include "Element.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoaderClientEA.h"
#include "FrameSelection.h"
#include "FrameTree.h"
#include "FrameView.h"
#if USE(JSC)
#include "GCController.h"
#elif USE(V8)
#include "V8GCController.h"
#endif
#include "GraphicsContext.h"
#include "HTMLMetaElement.h"
#include "HitTestResult.h"
#include "HTTPParsers.h"
#include "IconDatabase.h"
#include "InspectorController.h"
#if USE(JSC)
#include "JavaScript.h"
#include "JSDOMBinding.h"
#include "JSDOMWindowBase.h"
#include "JSLock.h"
#include "JSObject.h"
#include "JSRetainPtr.h"
#include "OpaqueJSString.h"
#elif USE(V8)
#include "V8DOMWrapper.h"
#include "V8DOMWindowShell.h"
#endif
#include "NetworkingContext.h"
#include "NodeList.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PlatformWheelEvent.h"
#include "PrintContext.h"
#if USE(JSC)
#include "PutPropertySlot.h"
#endif
#include "RenderLayer.h"
#include "RenderTreeAsText.h"
#include "RenderView.h"
#include "ResourceRequest.h"
#include "ScriptController.h"
#include "ScriptSourceCode.h"
#include "ScriptValue.h"
#include "Scrollbar.h"
#include "Settings.h"
#include "SubstituteData.h"
#include "SVGSMILElement.h"
#include "TiledBackingStore.h"
#include "htmlediting.h"
#include "markup.h"

#include "WebFrame_p.h"
#include "WebPage.h"
#include "WebPage_p.h"
#if USE(JSC)
#include "runtime_object.h"
#include "runtime_root.h"
#include "interpreter.h"
#endif
#if USE(ACCELERATED_COMPOSITING)
#include "texmap/TextureMapper.h"
#include "texmap/TextureMapperPlatformLayer.h"
#endif

#include "wtf/HashMap.h"
#include "ea_utility.h"
#include "ea_instance.h"
#include <EAWebKit/EAWebKit.h>
#include <EAWebkit/EAWebkitClient.h>
#include "PageClientEA.h"

#include "ScrollTypes.h"
#include "RefPtrCairo.h"

#include "PlatformContextCairo.h"

namespace EA
{
namespace WebKit
{
    const bool kZoomTextOnly = false;   // QT had this as a user option, not sure we would even need it.  
    
    WebFrameData::WebFrameData(WebCore::Page* parentPage, WebCore::Frame* parentFrame,
                             WebCore::HTMLFrameOwnerElement* ownerFrameElement,
                             const WTF::String& frameName)
    : name(frameName)
    , ownerElement(ownerFrameElement)
    , page(parentPage)
    , allowsScrolling(true)
    , marginWidth(0)
    , marginHeight(0)
{
	using namespace WebCore;
	frameLoaderClient = new FrameLoaderClientEA();
	frame = Frame::create(page, ownerElement, frameLoaderClient);

	// FIXME: All of the below should probably be moved over into WebCore
	frame->tree()->setName(name);
	if (parentFrame)
		parentFrame->tree()->appendChild(frame);
}


void WebFramePrivate::init(WebFrame* wframe, WebFrameData* frameData)
{
	webFrame = wframe;
	
	allowsScrolling = frameData->allowsScrolling;
	marginWidth = frameData->marginWidth;
	marginHeight = frameData->marginHeight;
	frame = frameData->frame.get();
	frameLoaderClient = frameData->frameLoaderClient;
	frameLoaderClient->setFrame(wframe, frame);

	frame->init();
}

void WebFramePrivate::setPage(WebPage* newPage)
{
// 	if (page == newPage)
// 		return;
	EAW_ASSERT_MSG(!page, "Not expecting it to be called multiple times");
	page = newPage;
}

#if USE(ACCELERATED_COMPOSITING)
void WebFramePrivate::renderCompositedLayers(WebCore::GraphicsContext *context, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
    int scrollX = frame->view()->scrollX();
    int scrollY = frame->view()->scrollY();
    context->translate(-scrollX, -scrollY);

    // The entire screen is composited every tick. This is so that we don't have to render to 
    // a texture that contains the page, saving a couple MB of runtime memory.
    // The tradeoff is that we have to run through the compositing layers each frame, so there's
    // a slight CPU hit.
    WebCore::IntRect fullscreen(IntPoint(0, 0), WebCore::IntSize(page->view()->GetSize()));
    WebCore::IntRect scrolledScreen(IntPoint(scrollX, scrollY), fullscreen.size());
    EA::WebKit::TransformationMatrix identity;
    const WebCore::TransformationMatrix webcoreIdentity(1, 0, 0, 0,
                                                        0, 1, 0, 0,
                                                        0, 0, 1, 0,
                                                        0, 0, 0, 1);

    IHardwareRenderer *renderer = page->view()->GetHardwareRenderer();
    
#if ENABLE(TILED_BACKING_STORE)
    if(page->view()->IsUsingTiledBackingStore())
	{
		NOTIFY_PROCESS_STATUS(kVProcessTypePaintTilesGPU, EA::WebKit::kVProcessStatusStarted, page->view());
    
		// Render the tiled backing store.
		EA::WebKit::FloatRect floatClip(fullscreen.x(), fullscreen.y(), fullscreen.width(), fullscreen.height());
		renderer->BeginClip(identity,floatClip);
		frame->tiledBackingStore()->paint(context, scrolledScreen);
		renderer->EndClip();
	    
		NOTIFY_PROCESS_STATUS(kVProcessTypePaintTilesGPU, EA::WebKit::kVProcessStatusEnded, page->view());
	}
	else
	{
		webFrame->drawHighlightedNodeFromInspector(page->view()->GetDisplaySurface());

		EA::WebKit::FloatRect floatClip(fullscreen.x(), fullscreen.y(), fullscreen.width(), fullscreen.height());
		renderer->BeginClip(identity, floatClip);
		EA::WebKit::FloatRect target(0.0f, 0.0f, fullscreen.width(), fullscreen.height());
		EA::WebKit::TransformationMatrix transform(context->getCTM().toTransformationMatrix().translate(scrollX, scrollY));
		renderer->RenderSurface(page->view()->GetDisplaySurface(), target, transform, 1.0f, NULL);
		renderer->EndClip();
	}
#endif

    // Render composited content if it exists.
	// Following or frame->view()->hasCompositedContent() can be used to tell if the page is currently compositing anything. Need to use it.
    if (textureMapper && rootTextureMapperNode)
    {
        NOTIFY_PROCESS_STATUS(kVProcessTypePaintTextureMapper, EA::WebKit::kVProcessStatusStarted, page->view());
        
        textureMapper->setGraphicsContext(context);
        textureMapper->setImageInterpolationQuality(context->imageInterpolationQuality());
        textureMapper->setTextDrawingMode(context->textDrawingMode());
        textureMapper->setViewportSize(frame->view()->frameRect().size());
        PlatformGraphicsContext *platformContext = context->platformContext();

        rootTextureMapperNode->setTransform(webcoreIdentity);
        rootTextureMapperNode->setOpacity(platformContext->globalAlpha());

        textureMapper->beginClip(webcoreIdentity, fullscreen);
        rootTextureMapperNode->paint();
        textureMapper->endClip();

        NOTIFY_PROCESS_STATUS(kVProcessTypePaintTextureMapper, EA::WebKit::kVProcessStatusEnded, page->view());
    }


    NOTIFY_PROCESS_STATUS(kVProcessTypePaintScrollbars, EA::WebKit::kVProcessStatusStarted, page->view());

    // Paint/render scrollbars.
    renderScrollbar(horizontalScrollBar(), renderer, &mHorizontalScroll, dirtyRegions);
    renderScrollbar(verticalScrollBar(), renderer, &mVerticalScroll, dirtyRegions);
    renderScrollCorner(frame->view(), renderer, &mScrollCorner, dirtyRegions);

    NOTIFY_PROCESS_STATUS(kVProcessTypePaintScrollbars, EA::WebKit::kVProcessStatusEnded, page->view());
}

void WebFramePrivate::renderScrollCorner(WebCore::FrameView *view, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
    if (view && view->scrollbarCornerPresent())
    {
        bool needsPaint = false;
        if (!*surface)
        {
            *surface = renderer->CreateSurface();
            needsPaint = true;
        }

        const WebCore::IntRect scrollCornerRect = view->scrollCornerRect();
        for (size_t i = 0; i < dirtyRegions.size(); ++i)
        {
            if (dirtyRegions[i].intersects(scrollCornerRect))
            {
                needsPaint = true;
                break;
            }
        }

        if (needsPaint)
        {
            // Just paint the entire thing.
            (*surface)->SetContentDimensions(scrollCornerRect.width(), scrollCornerRect.height());

            ISurface::SurfaceDescriptor desc;
            (*surface)->Lock(&desc);

            RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)desc.mData, CAIRO_FORMAT_ARGB32, scrollCornerRect.width(), scrollCornerRect.height(), desc.mStride));    
            RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

            cairo_surface_set_device_offset(cairoSurface.get(), -scrollCornerRect.x(), -scrollCornerRect.y()); 

            WebCore::GraphicsContext graphicsContext(cairoContext.get());
            view->paintScrollCorner(&graphicsContext, scrollCornerRect);

            (*surface)->Unlock();
        }

        // Send it to the GPU.
        EA::WebKit::TransformationMatrix identity;
        EA::WebKit::FloatRect scrollCornerFloatRect(scrollCornerRect.x(), scrollCornerRect.y(), scrollCornerRect.width(), scrollCornerRect.height());
        renderer->BeginClip(identity, scrollCornerFloatRect);
        renderer->RenderSurface(*surface, scrollCornerFloatRect, identity, 1.0f, NULL);
        renderer->EndClip();
    }
}

void WebFramePrivate::renderScrollbar(WebCore::Scrollbar *bar, IHardwareRenderer *renderer, ISurface **surface, const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
    if (bar)
    {
        bool needsPaint = false;
        if (!*surface)
        {
            *surface = renderer->CreateSurface();
            needsPaint = true;
        }

        const WebCore::IntRect scrollbarRect = bar->frameRect();
        for (size_t i = 0; i < dirtyRegions.size(); ++i)
        {
            if (dirtyRegions[i].intersects(scrollbarRect))
            {
                needsPaint = true;
                break;
            }
        }

        if (needsPaint)
        {
            // Just paint the entire thing.
            (*surface)->SetContentDimensions(scrollbarRect.width(), scrollbarRect.height());

            ISurface::SurfaceDescriptor desc;
            (*surface)->Lock(&desc);

            RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)desc.mData, CAIRO_FORMAT_ARGB32, scrollbarRect.width(), scrollbarRect.height(), desc.mStride));    
            RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

            cairo_surface_set_device_offset(cairoSurface.get(), -scrollbarRect.x(), -scrollbarRect.y()); 

            WebCore::GraphicsContext graphicsContext(cairoContext.get());
            bar->paint(&graphicsContext, scrollbarRect);

            (*surface)->Unlock();
        }

        // Send it to the GPU.
        EA::WebKit::TransformationMatrix identity;
        EA::WebKit::FloatRect scrollbarFloatRect(scrollbarRect.x(), scrollbarRect.y(), scrollbarRect.width(), scrollbarRect.height());
        renderer->BeginClip(identity, scrollbarFloatRect);
        renderer->RenderSurface(*surface, scrollbarFloatRect, identity, 1.0f, NULL);
        renderer->EndClip();
    }
}

#endif

WebCore::Scrollbar* WebFramePrivate::horizontalScrollBar() const
{
	if (!frame->view())
		return 0;
	return frame->view()->horizontalScrollbar();
}

WebCore::Scrollbar* WebFramePrivate::verticalScrollBar() const
{
	if (!frame->view())
		return 0;
	return frame->view()->verticalScrollbar();
}

void WebFramePrivate::urlChanged()
{
	url = frame->document()->url();
}

WebFrame::WebFrame(WebPage *parent, WebFrameData *frameData)
: d(new WebFramePrivate)
{
	d->page = parent;
	d->init(this, frameData);

	if (!frameData->url.isEmpty()) {
		WebCore::ResourceRequest request(frameData->url, frameData->referrer);
		d->frame->loader()->load(request, frameData->name, false);
	}
}

WebFrame::WebFrame(WebFrame *parent, WebFrameData *frameData)
: d(new WebFramePrivate)
{
	d->page = parent->d->page;
	// abaldeva: Qt knows parentFrame through their framework. Also, in Qt, the parent of a WebFrame does not need to be WebFrame, 
	// it can be a WebPage. We set it explicitly here.
	d->parentWebFrame = parent;
	d->init(this, frameData);
}

WebFrame::~WebFrame()
{
	if (d->frame && d->frame->loader() && d->frame->loader()->client())
    {
		static_cast<WebCore::FrameLoaderClientEA*>(d->frame->loader()->client())->m_webFrame = 0;
    }

    if (d->mHorizontalScroll)
    {
        d->mHorizontalScroll->Release();
    }

    if (d->mVerticalScroll)
    {
        d->mVerticalScroll->Release();
    }

    if (d->mScrollCorner)
    {
        d->mScrollCorner->Release();
    }

    delete d;
}


void WebFrame::addToJavaScriptWindowObject(const char8_t *name, IJSBoundObject *obj)
{
    JSC::JSLock lock(JSC::SilenceAssertionsOnly);
    WebCore::JSDOMWindow* window = toJSDOMWindow(d->frame, WebCore::mainThreadNormalWorld());
    JSC::Bindings::RootObject* root = d->frame->script()->bindingRootObject();;

    if (!window) 
	{
        EAW_ASSERT_MSG(false, "Could not get window object");
		return;
    }

	if (!root) 
	{
		EAW_ASSERT_MSG(false, "Could not get root object");
		return;
	}

	JSC::ExecState* exec = window->globalExec();
    JSC::JSObject* runtimeObject = JSC::Bindings::EAInstance::create(obj, root)->createRuntimeObject(exec);

    JSC::Identifier ident(exec, name);
    JSC::PutPropertySlot slot;
    window->put(exec, ident, runtimeObject, slot);
}

void WebFrame::GetJavaScriptCallstack(eastl::vector<eastl::string8> *namesOut, eastl::vector<eastl::string8> *argsOut, eastl::vector<int> *linesOut, eastl::vector<eastl::string8> *urlsOut)
{
    JSC::JSLock lock(JSC::SilenceAssertionsOnly);
    WebCore::JSDOMWindow* window = toJSDOMWindow(d->frame, WebCore::mainThreadNormalWorld());
    JSC::ExecState *exec = window->globalExec();

    exec->interpreter()->GetCallstack(exec, namesOut, argsOut, linesOut, urlsOut);
}

WTF::String WebFrame::toHtml() const
{
	if (!d->frame->document())
		return WTF::String();
	return WebCore::createMarkup(d->frame->document());
}

WTF::String WebFrame::toPlainText() const
{
	if (d->frame->view() && d->frame->view()->layoutPending())
		d->frame->view()->layout();

	WebCore::Element *documentElement = d->frame->document()->documentElement();
	if (documentElement)
		return documentElement->innerText();
	return WTF::String();
}

WTF::String WebFrame::renderTreeDump() const
{
	if (d->frame->view() && d->frame->view()->layoutPending())
		d->frame->view()->layout();

	return WebCore::externalRepresentation(d->frame);
}

WTF::String WebFrame::title() const
{
	if (d->frame->document())
		return d->frame->loader()->documentLoader()->title().string();
	return WTF::String();
}

static inline void clearCoreFrame(WebCore::Frame* frame)
{
	WebCore::DocumentLoader* documentLoader = frame->loader()->activeDocumentLoader();
	EAW_ASSERT(documentLoader);
	if(documentLoader)
	{
		documentLoader->writer()->begin();
		documentLoader->writer()->end();
	}
}

static inline bool isCoreFrameClear(WebCore::Frame* frame)
{
	return frame->document()->url().isEmpty();
}


void WebFrame::setUrl(const WebCore::KURL& url)
{
    clearCoreFrame(d->frame);
    d->url = url;
    load(url);
}

WebCore::KURL WebFrame::url() const
{
	return d->url;
}

WebCore::KURL WebFrame::requestedUrl() const
{
	return d->frameLoaderClient->lastRequestedUrl();
}

WebCore::KURL WebFrame::baseUrl() const
{
	if (isCoreFrameClear(d->frame))
		return d->url;

	return d->frame->document()->baseURL();
}

WebPage* WebFrame::page() const
{
	return d->page;
}

//The view remains the same until enough data has arrived to display the new url.
void WebFrame::load(const WebCore::KURL& url)
{
	if (d->parentFrame())
		d->page->d->insideOpenCall = true;

	d->frame->loader()->load(WebCore::ResourceRequest(url), false);

	if (d->parentFrame())
		d->page->d->insideOpenCall = false;
}

void WebFrame::setHtml(const char8_t* html, size_t length, const WebCore::KURL& baseUrl)
{
	setContent(html,length,"text/html; charset=utf-8",baseUrl);
}

void WebFrame::setContent(const char8_t* data, size_t length, const char8_t* pMediaType, const WebCore::KURL& baseUrl)
{
	WebCore::ResourceRequest request(baseUrl);
	WTF::RefPtr<WebCore::SharedBuffer> buffer = WebCore::SharedBuffer::create(data, length);
	WTF::String actualMimeType;
	WTF::String encoding;
	WTF::String mediaType(pMediaType);
	if (mediaType.isEmpty())
		actualMimeType = "text/html";
	else {
		actualMimeType = WebCore::extractMIMETypeFromMediaType(mediaType);
		encoding = WebCore::extractCharsetFromMediaType(mediaType);
	}
	WebCore::SubstituteData substituteData(buffer, actualMimeType, encoding, WebCore::KURL());
	d->frame->loader()->load(request, substituteData, false);
}






WebFrame *WebFrame::parentFrame() const
{
    return d->parentFrame();
}

void WebFrame::setParentFrame(WebFrame* wFrame)
{
	d->setParentFrame(wFrame);
}
EA::WebKit::ScrollbarPolicy WebFrame::scrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation) const
{
	if (orientation == EA::WebKit::ScrollbarHorizontal)
        return d->horizontalScrollBarPolicy;
    return d->verticalScrollBarPolicy;
}

void WebFrame::setScrollBarPolicy(EA::WebKit::ScrollbarOrientation orientation, EA::WebKit::ScrollbarPolicy policy)
{
	COMPILE_ASSERT((int)WebCore::ScrollbarAuto == (int)EA::WebKit::ScrollbarAuto, WebCoreAndEAWebKitScrollBarEnumsShouldMatch);
	COMPILE_ASSERT((int)WebCore::ScrollbarAlwaysOff == (int)EA::WebKit::ScrollbarAlwaysOff,WebCoreAndEAWebKitScrollBarEnumsShouldMatch);
    COMPILE_ASSERT((int)WebCore::ScrollbarAlwaysOn == (int)EA::WebKit::ScrollbarAlwaysOn,WebCoreAndEAWebKitScrollBarEnumsShouldMatch);

	if (orientation == EA::WebKit::ScrollbarHorizontal) {
        d->horizontalScrollBarPolicy = policy;
        if (d->frame->view()) {
			d->frame->view()->setHorizontalScrollbarMode((WebCore::ScrollbarMode)policy, policy != EA::WebKit::ScrollbarAuto /* lock */);
            d->frame->view()->updateCanHaveScrollbars();
        }
    } else {
        d->verticalScrollBarPolicy = policy;
        if (d->frame->view()) {
            d->frame->view()->setVerticalScrollbarMode((WebCore::ScrollbarMode)policy, policy != EA::WebKit::ScrollbarAuto /* lock */);
            d->frame->view()->updateCanHaveScrollbars();
        }
    }
}

void WebFrame::setScrollBarValue(EA::WebKit::ScrollbarOrientation orientation, int value)
{
	COMPILE_ASSERT((int)WebCore::HorizontalScrollbar	== (int)EA::WebKit::ScrollbarHorizontal,	ScrollbarOrientationEnumOutOfSync);
	COMPILE_ASSERT((int)WebCore::VerticalScrollbar		== (int)EA::WebKit::ScrollbarVertical,		ScrollbarOrientationEnumOutOfSync);

	WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb) {
        if (value < 0)
            value = 0;
        else if (value > scrollBarMaximum(orientation))
            value = scrollBarMaximum(orientation);
        sb->scrollableArea()->scrollToOffsetWithoutAnimation(orientation == EA::WebKit::ScrollbarHorizontal ? WebCore::HorizontalScrollbar : WebCore::VerticalScrollbar, value);
    }
}

int WebFrame::scrollBarValue(EA::WebKit::ScrollbarOrientation orientation) const
{
	WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->value();
    return 0;
}

int WebFrame::scrollBarMaximum(EA::WebKit::ScrollbarOrientation orientation) const
{
    WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->maximum();
    return 0;
}

int WebFrame::scrollBarMinimum(EA::WebKit::ScrollbarOrientation orientation) const
{
    (void)orientation;
    return 0;
}

WebCore::IntRect WebFrame::scrollBarGeometry(EA::WebKit::ScrollbarOrientation orientation) const
{
    WebCore::Scrollbar *sb;
	sb = (orientation == EA::WebKit::ScrollbarHorizontal) ? d->horizontalScrollBar() : d->verticalScrollBar();
    if (sb)
        return sb->frameRect();
	return WebCore::IntRect();
}

void WebFrame::scroll(int dx, int dy)
{
    if (!d->frame->view())
        return;

    d->frame->view()->scrollBy(IntSize(dx, dy));
}

WebCore::IntPoint WebFrame::scrollPosition() const
{
    if (!d->frame->view())
		return WebCore::IntPoint(0,0);

	WebCore::IntSize ofs = d->frame->view()->scrollOffset();
	return WebCore::IntPoint(ofs.width(), ofs.height());
}

void WebFrame::setScrollPosition(const WebCore::IntPoint &pos)
{
	WebCore::IntPoint current = scrollPosition();
    int dx = pos.x() - current.x();
    int dy = pos.y() - current.y();
    scroll(dx, dy);
}

void WebFrame::scrollToAnchor(const WTF::String& anchor)
{
    WebCore::FrameView *view = d->frame->view();
    if (view)
        view->scrollToAnchor(anchor);
}

void WebFrame::ClearDisplay(const WebCore::Color &color)
{
    d->mClearDisplaySurface = true;
    d->mClearDisplayColor = color;
}
#if USE(ACCELERATED_COMPOSITING)
void WebFrame::renderAccelerated(const eastl::vector<WebCore::IntRect> &dirtyRegions)
{
	WebCore::GraphicsContext* dummyGC = d->dummyGraphicsContext.get();
	
	dummyGC->save();
    d->renderCompositedLayers(dummyGC, dirtyRegions);
	dummyGC->restore();
}
#endif

void WebFrame::render(ISurface *surface, const eastl::vector<WebCore::IntRect> &dirtyRegions) 
{
    // A flag is used because we want the clear to
    // happen from within the render notifications.
    if (d->mClearDisplaySurface)
    {
        int width = 0;
        int height = 0;
        surface->GetContentDimensions(&width, &height);

        ISurface::SurfaceDescriptor surfaceDescriptor = {0};
        surface->Lock(&surfaceDescriptor);

        RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, width, height, surfaceDescriptor.mStride));    
        RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

        WebCore::Color clearColor = d->mClearDisplayColor;
        cairo_save(cairoContext.get());
        
        cairo_set_source_rgba(cairoContext.get(), clearColor.red(), clearColor.green(), clearColor.blue(), clearColor.alpha());
        cairo_rectangle(cairoContext.get(), 0, 0, width, height);
        cairo_set_operator(cairoContext.get(), CAIRO_OPERATOR_SOURCE);
        cairo_fill(cairoContext.get());

        cairo_restore(cairoContext.get());

        surface->Unlock();

        d->mClearDisplaySurface = false;
    }

    WebCore::FrameView *view = d->frame->view();
    for (unsigned i = 0; i < dirtyRegions.size(); ++i) 
	{
        ISurface::SurfaceDescriptor surfaceDescriptor = {0};
        
        // Only lock the portion of the surface that is being rendered to.
        IntRect eaRect(dirtyRegions[i]);
        surface->Lock(&surfaceDescriptor, &eaRect);
        
		RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, eaRect.mSize.mWidth, eaRect.mSize.mHeight, surfaceDescriptor.mStride));    
        RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));
        
		WebCore::GraphicsContext graphicsContext(cairoContext.get());

        if (!graphicsContext.paintingDisabled() || graphicsContext.updatingControlTints()) 
        {
            // Translate the context so the dirty region is the origin.
            const WebCore::IntRect &dirty = dirtyRegions[i];
            
            // We use this device offset rather than translate so that functions like cairo_device_to_user() don't need to correct for the dirty rect offset.
            cairo_surface_set_device_offset(cairoSurface.get(), -dirty.x(), -dirty.y()); 

            // Paint contents and scroll bars.
            view->paint(&graphicsContext, dirty);


            // Uncomment to test cairo - Note: this will only render whatever is inside the dirty region, not the full page.
            //cairo_surface_flush(cairoSurface.get());
            //cairo_surface_write_to_png(cairoSurface.get(), "test.png");
        }

        if(d->page->view()->ShouldDrawDebugVisuals())
		{
			cairo_save(cairoContext.get());
			cairo_set_source_rgb(cairoContext.get(), 0, 255, 0);
			cairo_set_line_width(cairoContext.get(), 6);
			cairo_rectangle(cairoContext.get(), eaRect.mLocation.mX, eaRect.mLocation.mY, eaRect.mSize.mWidth, eaRect.mSize.mHeight);
			cairo_stroke(cairoContext.get());
			cairo_restore(cairoContext.get());
		}

        surface->Unlock();
    }

	drawHighlightedNodeFromInspector(surface);

}

void WebFrame::drawHighlightedNodeFromInspector(EA::WebKit::ISurface* surface)
{
#if ENABLE(INSPECTOR)
	WebCore::Frame* coreFrame = d->core(this);
	if (coreFrame->page()->inspectorController()->highlightedNode()) 
	{
		int width = 0;
		int height = 0;
		surface->GetContentDimensions(&width, &height);
		
        ISurface::SurfaceDescriptor surfaceDescriptor = {0};
		surface->Lock(&surfaceDescriptor);

		RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, width, height, surfaceDescriptor.mStride));    
		RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));
		
		WebCore::GraphicsContext graphicsContext(cairoContext.get());
		coreFrame->page()->inspectorController()->drawHighlight(graphicsContext);

		surface->Unlock();
	}
#endif
}

void WebFrame::setTextSizeMultiplier(float factor)
{
    // Note: QT sets the mZoomTextEnabled param here but we  want to leave it up to the global params
    //EA::WebKit::Parameters &params = EA::WebKit::GetParameters();
    //params.mZoomTextEnabled = true;
	d->frame->setPageAndTextZoomFactors(1, factor);
}

float WebFrame::textSizeMultiplier() const
{
    return  kZoomTextOnly ? d->frame->textZoomFactor() : d->frame->pageZoomFactor();
}

void WebFrame::setZoomFactor(float factor)
{
    if (kZoomTextOnly)
		d->frame->setTextZoomFactor(factor);
	else
		d->frame->setPageZoomFactor(factor);
}

float WebFrame::zoomFactor() const
{
	return kZoomTextOnly ? d->frame->textZoomFactor() : d->frame->pageZoomFactor();
}

bool WebFrame::hasFocus() const
{
    WebCore::Frame* ff = d->frame->page()->focusController()->focusedFrame();
    return ff && WebFramePrivate::kit(ff) == this;
}

void WebFrame::setFocus()
{
    WebFramePrivate::core(this)->page()->focusController()->setFocusedFrame(WebFramePrivate::core(this));
}

/*!
    Returns the position of the frame relative to it's parent frame.
*/
WebCore::IntPoint WebFrame::pos() const
{
    if (!d->frame->view())
		return WebCore::IntPoint();

    return d->frame->view()->frameRect().location();
}

/*!
    Return the geometry of the frame relative to it's parent frame.
*/
WebCore::IntRect WebFrame::geometry() const
{
    if (!d->frame->view())
        return WebCore::IntRect();
    return d->frame->view()->frameRect();
}

/*!
    the size of the contents in this frame
*/
WebCore::IntSize WebFrame::contentsSize() const
{
	WebCore::FrameView *view = d->frame->view();
    if (!view)
        return WebCore::IntSize();
    return view->contentsSize();
}

/*!
    Performs a hit test on the frame contents at the given position \a pos and returns the hit test result.
*/
WebHitTestResult WebFrame::hitTestContent(const WebCore::IntPoint &pos, bool excludeScrollbarFromHitTest) const
{
    if (!d->frame->view() || !d->frame->contentRenderer())
        return WebHitTestResult();

	WebCore::HitTestResult result = d->frame->eventHandler()->hitTestResultAtPoint(d->frame->view()->windowToContents(pos), /*allowShadowContent*/ false, /*ignoreClipping*/ true);

    // Added exclude scrollbar bool to allow inspector to launch even if the cursor is over a scrollbar.
    if ((excludeScrollbarFromHitTest) && (result.scrollbar()))
        return WebHitTestResult();

    return WebHitTestResult(new WebHitTestResultPrivate(result));
}



/*!
    Evaluates the JavaScript defined by \a scriptSource using this frame as context
    and returns the result of the last executed statement.
*/
bool WebFrame::evaluateJavaScript(const WTF::String& scriptSource, JavascriptValue *resultOut)
{
    WebCore::ScriptController *proxy = d->frame->script();
   
    if (proxy) 
    {
        WebCore::ScriptValue sv = proxy->executeScript(WebCore::ScriptSourceCode(scriptSource));

        if (!sv.hasNoValue()) 
        {
            JSC::JSValue v = sv.jsValue();
            JSC::ExecState *exec = proxy->globalObject(WebCore::mainThreadNormalWorld())->globalExec(); 

            if (resultOut) 
            {
                resultOut->SetExec(exec);
                JSC::Bindings::JStoEA(exec, v, resultOut);
            }

            return true;
        }
    }

    return false;
}

void *WebFrame::GetExec(void) 
{
    WebCore::ScriptController *proxy = d->frame->script();
    if (proxy) {
        return proxy->globalObject(WebCore::mainThreadNormalWorld())->globalExec();
    }

    return NULL;
}

WebFrame::LoadState WebFrame::GetLoadState() const
{
	return d->mLoadState;
}

void WebFrame::SetLoadState(LoadState loadState)
{
	d->mLoadState = loadState;
}

WebCore::Frame* WebFramePrivate::core(const WebFrame* webFrame)
{
	return webFrame->d->frame;
}

WebFrame* WebFramePrivate::kit(const WebCore::Frame* coreFrame)
{
	if(coreFrame->loader()->networkingContext())
		return static_cast<WebFrame*>(coreFrame->loader()->networkingContext()->originatingObject());
	return NULL;
}


WebHitTestResult::WebHitTestResult(WebHitTestResultPrivate *priv)
: d(priv)
{
}
WebHitTestResultPrivate::WebHitTestResultPrivate(const WebCore::HitTestResult &hitTest)
: isContentEditable(false)
, isContentSelected(false)
, isScrollBar(false)
{
	if (!hitTest.innerNode())
		return;
	// Leaving some example code here for now. We may end up resurrecting it through some other work we have in the 
	// dev plan.
#if 0
	pos = hitTest.point();
	WebCore::TextDirection dir;
	title = hitTest.title(dir);
	linkText = hitTest.textContent();
	linkUrl = hitTest.absoluteLinkURL();
	linkTitle = hitTest.titleDisplayString();
	alternateText = hitTest.altDisplayString();
	imageUrl = hitTest.absoluteImageURL();
#endif
	innerNode = hitTest.innerNode();
	innerNonSharedNode = hitTest.innerNonSharedNode();

	isContentEditable = hitTest.isContentEditable();
	isContentSelected = hitTest.isSelected();
	isScrollBar = hitTest.scrollbar();

	if (innerNonSharedNode && innerNonSharedNode->document()
		&& innerNonSharedNode->document()->frame())
		frame = WebFramePrivate::kit(innerNonSharedNode->document()->frame());

}







WebHitTestResult::WebHitTestResult()
: d(0)
{
}

WebHitTestResult::WebHitTestResult(const WebHitTestResult &other)
: d(0)
{
	if (other.d)
		d = new WebHitTestResultPrivate(*other.d);
}

WebHitTestResult &WebHitTestResult::operator=(const WebHitTestResult &other)
{
	if (this != &other) {
		if (other.d) {
			if (!d)
				d = new WebHitTestResultPrivate;
			*d = *other.d;
		} else {
			delete d;
			d = 0;
		}
	}
	return *this;
}

WebHitTestResult::~WebHitTestResult()
{
	delete d;
}

bool WebHitTestResult::isNull() const
{
	return !d;
}

WebFrame *WebHitTestResult::frame() const
{
	if (!d)
		return 0;
	return d->frame;
}

WebCore::Node* WebHitTestResult::node() const
{
	if (!d)
		return 0;
	return d->innerNode.get();
}

}
}

