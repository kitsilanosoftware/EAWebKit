/*
Copyright (C) 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

#if USE(ACCELERATED_COMPOSITING)

#include "TextureMapperEA.h"
#include "PlatformContextCairo.h"

#include <eastl/string.h>

#include "FloatPoint3d.h"
#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitHardwareRenderer.h>
#include <EAWebkit/EAWebKitSurface.h>

namespace WebCore {

//
// BitmapTextureEA
//
BitmapTextureEA::BitmapTextureEA(EA::WebKit::ISurface *surface, EA::WebKit::View* view)
: mView(view)
, mSurface(surface)
, mCairoSurface(NULL)
, mCairoContext(NULL)
, mContext(NULL)
{
    mView->AddTexture(this);
}

BitmapTextureEA::~BitmapTextureEA(void) 
{
    destroy();
	mView->RemoveTexture(this);
}

void BitmapTextureEA::destroy(void)
{
    if (mSurface)
    {
        mSurface->Release();
        mSurface = NULL;
    }
}

IntSize BitmapTextureEA::size(void) const
{
    int width = 0;
    int height = 0;
    mSurface->GetContentDimensions(&width, &height);

    return IntSize(width, height);
}

void BitmapTextureEA::reset(const IntSize& size, bool opaque)
{
    BitmapTexture::reset(size, opaque);

    int width = 0;
    int height = 0;
    mSurface->GetContentDimensions(&width, &height);
    if (size.width() != width || size.height() != height)
    {
        mSurface->SetContentDimensions(size.width(), size.height());
    }
	// We are not using opaque value in this function. Qt uses that to figure out if it should fill the surface with a transparent black or not.
	// The concept of Reset is a bit overloaded right now. This needs to be fixed.
	mSurface->Reset();
}

bool BitmapTextureEA::isValid(void) const
{
    return mSurface && mSurface->IsAllocated();
}

// This function locks the ISurface, and returns a cairo rendering context associated
// with the locked region.
PlatformGraphicsContext *BitmapTextureEA::beginPaint(const IntRect &dirtyRect)
{
    mDirtyRect = dirtyRect;

    EA::WebKit::ISurface::SurfaceDescriptor descriptor = {0};
    EA::WebKit::IntRect eaDirtyRect(dirtyRect);
    mSurface->Lock(&descriptor, &eaDirtyRect);

    mCairoSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)descriptor.mData, CAIRO_FORMAT_ARGB32, dirtyRect.width(), dirtyRect.height(), descriptor.mStride));
    cairo_surface_set_device_offset(mCairoSurface.get(), -dirtyRect.x(), -dirtyRect.y());
	mCairoContext = adoptRef(cairo_create(mCairoSurface.get()));
    mContext = new PlatformGraphicsContext(mCairoContext.get());

    // Clear out the region before drawing to it.
    cairo_save(mContext->cr());
    cairo_set_operator(mContext->cr(), CAIRO_OPERATOR_CLEAR);
    cairo_rectangle(mContext->cr(), dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
    cairo_fill(mContext->cr());
    cairo_restore(mContext->cr());

    cairo_set_operator(mContext->cr(), CAIRO_OPERATOR_OVER);
    return mContext;
}

void BitmapTextureEA::endPaint(void)
{
	// TextureMapperGL frees up the software backing once the surface is copied to the texture. For them, it happens
	// in this function.
	// Force the flush before the surface is unlocked.
	cairo_surface_flush(mCairoSurface.get());

	delete mContext;
    mContext = NULL;
    mCairoSurface = NULL;
	mCairoContext = NULL;

    mSurface->Unlock();
}

// Uses cairo to copy the contents of image into the ISurface.
// This function is not being called from anywhere but on surface, it seems to be doing opposite of the name. May be the implementation is wrong or the
// name should change to setContentsFromImage.
void BitmapTextureEA::setContentsToImage(Image *image)
{
    if (!image)
    {
        destroy();
        return;
    }

    // Make sure the ISurface is the correct size.
    RefPtr<cairo_surface_t> native = adoptRef(image->nativeImageForCurrentFrame());
    IntSize size(cairo_image_surface_get_width(native.get()), cairo_image_surface_get_height(native.get()));
    reset(size, false);

    // Lock and copy.
    EA::WebKit::ISurface::SurfaceDescriptor descriptor = {0};
    EA::WebKit::IntRect eaRect(EA::WebKit::IntPoint(0, 0), EA::WebKit::IntSize(size));
    mSurface->Lock(&descriptor, &eaRect);

    RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)descriptor.mData, CAIRO_FORMAT_ARGB32, size.width(), size.height(), descriptor.mStride));
    RefPtr<cairo_t> cr = adoptRef(cairo_create(mCairoSurface.get()));

    // Perform the copy operation using cairo.
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr.get(), native.get(), 0.0, 0.0);
    cairo_paint(cr.get());
    
    // Force the flush before the surface is unlocked.
    cairo_surface_flush(surface.get());

    mSurface->Unlock();
}

EA::WebKit::ISurface *BitmapTextureEA::GetSurface(void) const
{
    return mSurface;
}


//
// TextureMapperEA
//
PassOwnPtr<TextureMapper> TextureMapperEA::create(EA::WebKit::View *view)
{
    return adoptPtr(new TextureMapperEA(view));
}

TextureMapperEA::TextureMapperEA(EA::WebKit::View *view)
: mView(view)
, mRenderer(view->GetHardwareRenderer())
{
    // Do nothing.
}

void TextureMapperEA::drawTexture(const BitmapTexture& texture, const FloatRect& target, const TransformationMatrix& matrix, float opacity, const BitmapTexture *mask)
{
    
	const BitmapTextureEA *textureEA = static_cast<const BitmapTextureEA*>(&texture);
	const BitmapTextureEA *maskEA = static_cast<const BitmapTextureEA*>(mask);
	EA::WebKit::TransformationMatrix eaMatrix(matrix);
	EA::WebKit::FloatRect eaTarget(target);
	mRenderer->RenderSurface(textureEA->GetSurface(), eaTarget, eaMatrix, opacity, (maskEA && maskEA->isValid()) ? maskEA->GetSurface() : NULL);

	if(mView->ShouldDrawDebugVisuals())
	{
		// We create temporary surface here so that we can toggle the texture rects on/off easily. If we were to draw them in BitmapTextureEA::endPaint(),
		// we could not toggle them.
		// Another advantage seems to be that outlines seem more accurate when drawn here.
		
		EA::WebKit::ISurface::SurfaceDescriptor descriptor = {0};
		int width, height;
		textureEA->GetSurface()->GetContentDimensions(&width,&height);

		EA::WebKit::ISurface* debugSurface =  mRenderer->CreateSurface();
		debugSurface->SetContentDimensions(width,height);

		EA::WebKit::IntRect eaDirtyRect(0,0,width,height);
		debugSurface->Lock(&descriptor, &eaDirtyRect);

		RefPtr<cairo_surface_t> cairoDebugSurface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)descriptor.mData, CAIRO_FORMAT_ARGB32, width, height, descriptor.mStride));    
		RefPtr<cairo_t> cairoDebugContext = adoptRef(cairo_create(cairoDebugSurface.get()));

		cairo_save(cairoDebugContext.get());
		cairo_set_operator(cairoDebugContext.get(), CAIRO_OPERATOR_CLEAR);
		cairo_rectangle(cairoDebugContext.get(), 0, 0, width, height);
		cairo_fill(cairoDebugContext.get());
		cairo_restore(cairoDebugContext.get());

		//cairo_set_operator(cairoDebugContext.get(), CAIRO_OPERATOR_OVER);
		
		cairo_save(cairoDebugContext.get());
		cairo_set_source_rgb(cairoDebugContext.get(), 255, 0, 0);
		cairo_set_line_width(cairoDebugContext.get(), 3);
		cairo_rectangle(cairoDebugContext.get(), 0, 0, width, height);
		cairo_stroke(cairoDebugContext.get());
		cairo_restore(cairoDebugContext.get());
		cairo_surface_flush(cairoDebugSurface.get());

		debugSurface->Unlock();

		mRenderer->RenderSurface(debugSurface, eaTarget, eaMatrix, opacity, NULL);

		debugSurface->Release();
	}
}

void TextureMapperEA::bindSurface(BitmapTexture* texture)
{
    mRenderer->SetRenderTarget(texture ? static_cast<BitmapTextureEA*>(texture)->GetSurface() : NULL);
}

void TextureMapperEA::beginClip(const TransformationMatrix &transform, const FloatRect &region)
{
    EA::WebKit::TransformationMatrix eaMatrix(transform);
    EA::WebKit::FloatRect eaTarget(region);

    mRenderer->BeginClip(eaMatrix, eaTarget);
}

void TextureMapperEA::endClip(void) 
{
    mRenderer->EndClip();
}

void TextureMapperEA::setGraphicsContext(GraphicsContext* gc) 
{
    mContext = gc;
}

GraphicsContext *TextureMapperEA::graphicsContext(void) 
{
    return mContext;
}

PassRefPtr<BitmapTexture> TextureMapperEA::createTexture(void) 
{
    EA::WebKit::ISurface *surface = mRenderer->CreateSurface();
    return adoptRef(new BitmapTextureEA(surface,mView));
}

void TextureMapperEA::beginPainting(void)
{
    mRenderer->BeginPainting();
}

void TextureMapperEA::endPainting(void)
{
    mRenderer->EndPainting();
}

}

#endif
