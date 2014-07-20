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

#ifndef TEXTUREMAPPER_EA_H
#define TEXTUREMAPPER_EA_H

// Using TextureMapper interface is one of the ways to accomplish accelerated compositing in WebKit.
// Our port uses it this option and does not support accelerated compositing by other means(which some other platforms do).
#if USE(ACCELERATED_COMPOSITING)

#include <texmap/TextureMapper.h>
namespace EA
{
namespace WebKit
{
class View;
class ISurface;
class IHardwareRenderer;
}
}

namespace WebCore {

// Represents a surface on which graphics context can draw. Every composited element on the page gets its own BitmapTexture and the texture
// is composited with the rest of the page(other textures and common surface for drawing non-composited elements).
// This class manages the ISurface instance, as well as the cairo code required to render to it.
// It's mostly pass-through code, with some cairo glue code mixed in.
// It is a texture on which the an element suitable for accelerated compositing draws to. 
class BitmapTextureEA : public BitmapTexture
{
public:
	BitmapTextureEA(EA::WebKit::ISurface *surface, EA::WebKit::View* view);
    ~BitmapTextureEA(void);

    virtual void destroy(void);
    virtual IntSize size(void) const;
    virtual bool isValid(void) const;
    virtual void reset(const IntSize& size, bool opaque = false);

    virtual PlatformGraphicsContext *beginPaint(const IntRect &dirtyRect);
    virtual void endPaint(void);
    virtual void setContentsToImage(Image *image);

    EA::WebKit::ISurface *GetSurface(void) const;

private:
	EA::WebKit::View* mView;
	EA::WebKit::ISurface *mSurface;
    RefPtr<cairo_surface_t> mCairoSurface;
	RefPtr<cairo_t> mCairoContext;
    PlatformGraphicsContext *mContext;
	IntRect mDirtyRect;// For debug draw
};

// This is our implementation of the TextureMapper. The TextureMapper
// class itself handles most of the logic. The "pre-painted" textures are composited using TextureMapper.
class TextureMapperEA : public TextureMapper 
{
public:
    static PassOwnPtr<TextureMapper> create(EA::WebKit::View *view);

    TextureMapperEA(EA::WebKit::View *view);

    // texture is the pre-painted texture needs to be composited.
	// target is the area of texture that needs to be composited. From what I have seen, it is usually the entire texture.
	// modelViewMatrix provides the transformation(location/rotation/scale) that needs to be applied before compositing of the texture.
	// The matrix is in the screen coordinates.
	// opacity - Self explanatory.
	// maskTexture - Self explanatory. 
	virtual void drawTexture(const BitmapTexture& texture, const FloatRect& target, const TransformationMatrix& modelViewMatrix, float opacity, const BitmapTexture* maskTexture);
    virtual void bindSurface(BitmapTexture* surface); // Makes a surface the render target.
    
    virtual void setGraphicsContext(GraphicsContext* gc);
    virtual GraphicsContext* graphicsContext(void);
    
    virtual void beginClip(const TransformationMatrix& transform, const FloatRect& region);
    virtual void endClip(void);
    
    virtual bool allowSurfaceForRoot(void) const { return true; }
    
    virtual PassRefPtr<BitmapTexture> createTexture(void);

    virtual void beginPainting(void);
    virtual void endPainting(void);

private:
	EA::WebKit::View* mView;
	EA::WebKit::IHardwareRenderer *mRenderer;
    GraphicsContext *mContext;
};

}

#endif
#endif
