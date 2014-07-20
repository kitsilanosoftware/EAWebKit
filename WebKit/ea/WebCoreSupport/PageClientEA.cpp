/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.
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
 *
 */

#include "config.h"

#include "PageClientEA.h"
#include "TextureMapperEA.h"
#include "texmap/TextureMapperPlatformLayer.h"
#include "WebFrame_p.h"

#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>

#include "TiledBackingStore.h"
#include "WebPage.h"
#include "Page.h"
#include "RenderBlock.h"
#include "RenderView.h"
#include "HTMLNames.h"

namespace WebCore {

#if USE(ACCELERATED_COMPOSITING)
TextureMapperNodeClientEA::TextureMapperNodeClientEA(EA::WebKit::WebFrame* frame, GraphicsLayer* layer)
: m_frame(frame)
, m_rootGraphicsLayer(GraphicsLayer::create(0))
{
    m_frame->d->rootTextureMapperNode = rootNode();
    m_rootGraphicsLayer->addChild(layer);
    m_rootGraphicsLayer->setDrawsContent(false);
    m_rootGraphicsLayer->setMasksToBounds(false);
    m_rootGraphicsLayer->setSize(IntSize(1, 1));
}

void TextureMapperNodeClientEA::setTextureMapper(const PassOwnPtr<TextureMapper>& textureMapper)
{
    m_frame->d->textureMapper = textureMapper;
    m_frame->d->rootTextureMapperNode->setTextureMapper(m_frame->d->textureMapper.get());
}

TextureMapperNodeClientEA::~TextureMapperNodeClientEA(void)
{
    m_frame->d->rootTextureMapperNode = 0;
}

void TextureMapperNodeClientEA::syncRootLayer(void)
{
    m_rootGraphicsLayer->syncCompositingStateForThisLayerOnly();
}

TextureMapperNode* TextureMapperNodeClientEA::rootNode(void)
{
    return toTextureMapperNode(m_rootGraphicsLayer.get());
}

void PageClientWebView::setRootGraphicsLayer(GraphicsLayer* layer)
{
    if (layer) {
        textureMapperNodeClient = adoptPtr(new TextureMapperNodeClientEA(page->mainFrame(), layer));
        textureMapperNodeClient->setTextureMapper(TextureMapperEA::create(page->view()));
        textureMapperNodeClient->syncRootLayer();
        return;
    }

    textureMapperNodeClient.clear();
}

void PageClientWebView::markForSync(bool scheduleSync)
{
    // mNeedsSync needs to be true regardless of the value of scheduleSync. This is similar to Qt port where a timer is fired regardless of the scheduleSync
	// boolean. In fact, scheduleSync is not handled at all.
	mNeedsSync = true;
}

void PageClientWebView::syncLayers(void)
{
 	if (!mNeedsSync) {
         return;
     }

    if (textureMapperNodeClient)
        textureMapperNodeClient->syncRootLayer();
    
    EA::WebKit::WebFramePrivate::core(page->mainFrame())->view()->syncCompositingStateIncludingSubframes();
    
    // This is not a 'permanent' true/false it changes as things in the page come to rest / start animating.
	if (textureMapperNodeClient)
		mNeedsSync = textureMapperNodeClient->rootNode()->descendantsOrSelfHaveRunningAnimations();    
}

bool PageClientWebView::allowsAcceleratedCompositing(void) const
{
    return view->HardwareAccelerated();
}
#endif

#if ENABLE(TILED_BACKING_STORE)
void PageClientWebView::removeNonVisibleTiles(WebCore::Timer<PageClientWebView>*)
{
	view->Page()->handle()->page->mainFrame()->tiledBackingStore()->removeAllNonVisibleTiles();
}
#endif
void PageClientWebView::scroll(int dx, int dy, const WebCore::IntRect& rectToScroll)
{
    view->AddDirtyRegion(rectToScroll);

#if ENABLE(TILED_BACKING_STORE)
	if (view->HardwareAccelerated() && view->IsUsingTiledBackingStore())
    {
		// Current tiled backing storage backend code has troubles with fixed elements and IFrames. We try to execute special case code here.
		// This code below has definite performance impact and one other option is to not use tiled backing storage at all.
		// FWIW, even the non-special code path down below shows performance regression on scrolling rather than any improvement. That needs to 
		// be investigated as well.

		bool useSlowRepaint = (dx == 0 && dy == 0);
        if (!useSlowRepaint)
        {
            // Iframe detection:
            // Element from point does a visible context check so we need to move the point to the visible area if needed.
            // We use the top left corner as a sample point.
            int x = rectToScroll.x() >= 0 ? rectToScroll.x() : 0;
            int y = rectToScroll.y() >= 0 ? rectToScroll.y() : 0;
            Element*  element = view->Page()->handle()->page->mainFrame()->document()->elementFromPoint(x,y);
            if (element)    
            {
                if (element->hasTagName(HTMLNames::iframeTag) || element->hasTagName(HTMLNames::frameTag)) 
                    useSlowRepaint = true;
            }
        
            // Fixed position detection:
            if (!useSlowRepaint)
		    {
			    RenderBlock::PositionedObjectsListHashSet* positionedObjects = 0;
			    if (RenderView* root = EA::WebKit::WebFramePrivate::core(page->mainFrame())->contentRenderer())
				    positionedObjects = root->positionedObjects();
			    if(positionedObjects)
			    {
				    RenderBlock::PositionedObjectsListHashSet::const_iterator end = positionedObjects->end();
				    for (RenderBlock::PositionedObjectsListHashSet::const_iterator it = positionedObjects->begin(); it != end; ++it) 
				    {
					    RenderBox* renderBox = *it;
					    if (renderBox->style()->position() != FixedPosition)
						    continue;
					    useSlowRepaint = true;
					    break;
				    }
			    }
            }
		}
		if (useSlowRepaint)
		{
			IntPoint currentPosition = page->mainFrame()->scrollPosition();
			IntRect r(currentPosition.x() + rectToScroll.x(), currentPosition.y() + rectToScroll.y(), rectToScroll.width(), rectToScroll.height());
			view->Page()->handle()->page->mainFrame()->tiledBackingStore()->invalidate(r);
		}
		else
		{
			// Only repaint the pieces of the tiles that need to be updated.
			IntPoint currentPosition = page->mainFrame()->scrollPosition();
			IntSize viewport = page->viewportSize();
			if (dy)
			{
				IntRect yRegion;
				if (dy < 0)
				{
					yRegion = IntRect(currentPosition.x(), currentPosition.y() + dy + viewport.height(), viewport.width(), -dy);
				}
				else if (dy > 0)
				{
					yRegion = IntRect(currentPosition.x(), currentPosition.y(), viewport.width(), dy);
				}

				view->Page()->handle()->page->mainFrame()->tiledBackingStore()->invalidate(yRegion);
			}

			if (dx)
			{
				IntRect xRegion;
				if (dx < 0)
				{
					xRegion = IntRect(currentPosition.x() + dx + viewport.width(), currentPosition.y(), -dx, viewport.height());
				}
				else if (dx > 0)
				{
					xRegion = IntRect(currentPosition.x(), currentPosition.y(), dy, viewport.height());
				}

				view->Page()->handle()->page->mainFrame()->tiledBackingStore()->invalidate(xRegion);
			}
		}

		if(view->ShouldRemoveNonVisibleTiles())
			mNonVisibleTilesRemovalTimer.startOneShot(0);

    }
#endif
}

void PageClientWebView::update(const WebCore::IntRect& dirtyRect)
{
	view->AddDirtyRegion(dirtyRect);
}

void PageClientWebView::setInputMethodEnabled(bool enable)
{
}

bool PageClientWebView::inputMethodEnabled() const
{
	return false;
}

PageClientWebView::~PageClientWebView()
{
}

void PageClientWebView::updateCursor(int type)
{
	using namespace EA::WebKit;
    if(EAWebKitClient* const pClient = GetEAWebKitClient(view))
    {
        CursorChangeInfo cci( view, view->GetUserData(), (EA::WebKit::CursorType) type); 
        pClient->CursorChanged(cci);
    }
}


int PageClientWebView::screenNumber() const
{
    return 0;
}

WebCore::IntRect PageClientWebView::geometryRelativeToOwnerWidget() const
{
    return WebCore::IntRect();
}

WebCore::IntRect PageClientWebView::windowRect() const
{
    return WebCore::IntRect();
}
} // namespace WebCore
