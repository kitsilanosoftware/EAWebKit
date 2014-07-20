/*
Copyright (C) 2012 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
its contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#if ENABLE(TILED_BACKING_STORE)

#include "TileEA.h"

#include "GraphicsContext.h"
#include "TiledBackingStore.h"
#include "TiledBackingStoreClient.h"

#include "Frame.h"
#include "Page.h"
#include "WebPage.h"
#include "WebFrame_p.h"

#include <EAWebkit/EAWebKitHardwareRenderer.h>
#include <EAWebkit/EAWebKitSurface.h>

#include "RefPtrCairo.h"
#include "PlatformContextCairo.h"

namespace WebCore {

TileEA::TileEA(TiledBackingStore* backingStore, const Coordinate& tileCoordinate, EA::WebKit::View *view)
: mBackingStore(backingStore)
, mCoordinate(tileCoordinate)
, mRect(mBackingStore->tileRectForCoordinate(tileCoordinate))
, mDirtyRect(mRect)
, mSurface(NULL)
, mRenderer(view->GetHardwareRenderer())
, mView(view)
{
    mView->AddTile(this);
}

TileEA::~TileEA(void)
{
    if (mSurface)
    {
        mSurface->Release();
        mSurface = NULL;
    }
	mView->RemoveTile(this);
}

bool TileEA::isDirty(void) const
{ 
    return !mDirtyRect.isEmpty();
}

bool TileEA::isReadyToPaint(void) const
{ 
    return mSurface != NULL;
}

void TileEA::invalidate(const IntRect& dirtyRect)
{
    IntRect tileDirtyRect = intersection(dirtyRect, mRect);
    if (!tileDirtyRect.isEmpty())
    {
        mDirtyRect.unite(tileDirtyRect);
        //mDirtyRect = mRect;
    }
}

Vector<IntRect> TileEA::updateBackBuffer(void)
{
    Vector<IntRect> returnRects;

    if (isDirty())
    {
        if (!mSurface)
        {
            mSurface = mRenderer->CreateSurface();
            mSurface->SetContentDimensions(mRect.width(), mRect.height());
        }

        EA::WebKit::ISurface::SurfaceDescriptor surfaceDescriptor = {0};
        // Only lock the portion of the surface that is being rendered to.
        EA::WebKit::IntRect eaRect(0, 0, mRect.width(), mRect.height());
        mSurface->Lock(&surfaceDescriptor, &eaRect);

        RefPtr<cairo_surface_t> cairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, eaRect.mSize.mWidth, eaRect.mSize.mHeight, surfaceDescriptor.mStride));    
        RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(cairoSurface.get()));

        WebCore::GraphicsContext graphicsContext(cairoContext.get());

        // The movie system calls back to the app and needs the offset of the dirty rect.  
        // It cannot extract it if it is combined with the general translation so we use the device offset instead.
        cairo_surface_set_device_offset(cairoSurface.get(), -mRect.x(), -mRect.y());

        graphicsContext.clip(mDirtyRect);
        graphicsContext.scale(FloatSize(mBackingStore->contentsScale(), mBackingStore->contentsScale()));
        mBackingStore->client()->tiledBackingStorePaint(&graphicsContext, mBackingStore->mapToContents(mDirtyRect));

		if(mView->ShouldDrawDebugVisuals())
		{
			const int kLineWidth = 3; 
			
			// Draw dirty rects first
			cairo_save(cairoContext.get());
			cairo_set_source_rgb(cairoContext.get(), 0, 255, 0);
			// The width is adjusted below to be thicker when drawing a normal dirty rect which is not full tile. When dirty rect is full tile, we use 
			// lower width so that the green lines get overridden by blue lines of the tile draw just below and we get nice solid blue lines for tiles.
			cairo_set_line_width(cairoContext.get(), ((mDirtyRect != mRect) ? 2*kLineWidth : kLineWidth));
			cairo_rectangle(cairoContext.get(), mDirtyRect.x(), mDirtyRect.y(), mDirtyRect.width(), mDirtyRect.height());
			cairo_stroke(cairoContext.get());

			// Draw tiles after dirty rects so that we can see them all the time.
			// Tiles appear to have same thickness even though their line width is half of dirty rects because they are adjacent to each other.
			cairo_set_source_rgb(cairoContext.get(), 0, 0, 255);
			cairo_set_line_width(cairoContext.get(), kLineWidth);
			cairo_rectangle(cairoContext.get(), mRect.x(), mRect.y(), mRect.width(), mRect.height());
			cairo_stroke(cairoContext.get());
			cairo_restore(cairoContext.get());
		}

        mSurface->Unlock();

        returnRects.append(mDirtyRect);
        mDirtyRect = IntRect();
    }

    return returnRects;
}

void TileEA::swapBackBufferToFront(void)
{
    // We don't need this.
}

void TileEA::paint(GraphicsContext* context, const IntRect& rect)
{
    if (mSurface)
    {
        IntRect subRect = intersection(rect, mRect);
        if (!subRect.isEmpty())
        {
            // Draw the entire tile.
            EA::WebKit::FloatRect target(0.0f, 0.0f, mRect.width(), mRect.height());
            EA::WebKit::TransformationMatrix transform(context->getCTM().toTransformationMatrix().translate(mRect.x(), mRect.y()));
            mRenderer->RenderSurface(mSurface, target, transform, 1.0f, NULL);
        }
    }
}

void TileEA::resize(const IntSize& newSize)
{
    if (mSurface)
    {
        mSurface->SetContentDimensions(newSize.width(), newSize.height());
    }


//    IntRect oldRect = mRect;
    mRect = IntRect(mRect.location(), newSize);
    invalidate(mRect);

    //if (mRect.maxX() > oldRect.maxX())
    //    invalidate(IntRect(oldRect.maxX(), oldRect.y(), mRect.maxX() - oldRect.maxX(), mRect.height()));
    //if (mRect.maxY() > oldRect.maxY())
    //    invalidate(IntRect(oldRect.x(), oldRect.maxY(), mRect.width(), mRect.maxY() - oldRect.maxY()));
}

void TiledBackingStoreBackend::paintCheckerPattern(GraphicsContext* context, const FloatRect& target)
{
    // EAWebkitTODO: Not sure if this is necessary.
}

PassRefPtr<Tile> TiledBackingStoreBackend::createTile(TiledBackingStore* backingStore, const Tile::Coordinate& tileCoordinate)
{
    // Looks like the only class that is a client is the Frame. If that changes this code will need to change.
    WebCore::Frame *frame = static_cast<WebCore::Frame*>(backingStore->client());
    EA::WebKit::WebPage *eaPage = EA::WebKit::WebFramePrivate::kit(frame)->page();
    EA::WebKit::View *view = eaPage->view();

    return TileEA::create(backingStore, tileCoordinate, view);
}

}
#endif
