/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
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
#include "BitmapImage.h"

#include <cairo/cairo.h>
#include <internal/include/EAWebKit_p.h>

namespace WebCore {

// Inline 16x16 "?" missing image icon binary. 
static const int kBrokenImageWidth = 16; 
static const int kBrokenImageHeight = 16; 
static const int brokenImageBin[] =
{
 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4, 0xff94b1d4
,0xff92afd4, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff92afd4
,0xff90aed3, 0xffffffff, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xff2f8cca, 0xffffffff, 0xff90aed3
,0xff8dacd2, 0xffffffff, 0xff2e8ac9, 0xff2e8ac9, 0xff2e8ac9, 0xff2c86c6, 0xff2c86c6, 0xff2c85c5, 0xff2c85c5, 0xff2c85c5, 0xff2c86c6, 0xff2e89c8, 0xff2e8ac9, 0xff2e8ac9, 0xffffffff, 0xff8dacd2
,0xff8aaad0, 0xffffffff, 0xff2d88c7, 0xff2d88c7, 0xff2b84c4, 0xff2a81c2, 0xff8ebde0, 0xff8ebde0, 0xffb9d6ec, 0xff9cc6e3, 0xff4692ca, 0xff2a81c2, 0xff2d88c7, 0xff2d88c7, 0xffffffff, 0xff8aaad0
,0xff87a7cf, 0xffffffff, 0xff2c86c5, 0xff2c86c5, 0xff2a83c2, 0xff8dbcde, 0xffe3eff7, 0xff2980c0, 0xff4691c8, 0xffffffff, 0xffe3eff7, 0xff297fc0, 0xff2c86c5, 0xff2c86c5, 0xffffffff, 0xff87a7cf
,0xff84a5ce, 0xffffffff, 0xff2b83c3, 0xff2b83c3, 0xff2a80c1, 0xff5498cc, 0xff8dbbdd, 0xff287dbf, 0xff3686c2, 0xffffffff, 0xfff1f7fb, 0xff287dbe, 0xff2b83c3, 0xff2b83c3, 0xffffffff, 0xff84a5ce
,0xff80a2cc, 0xffffffff, 0xff2a80c1, 0xff2a80c1, 0xff297fc0, 0xff297dbf, 0xff297dbf, 0xff287dbe, 0xff9bc2e1, 0xffc6ddee, 0xff5395ca, 0xff277abd, 0xff2a80c1, 0xff2a80c1, 0xffffffff, 0xff80a2cc
,0xff7da0cb, 0xffffffff, 0xff287dbf, 0xff287dbf, 0xff287dbf, 0xff287dbf, 0xff277cbe, 0xff5295ca, 0xff8cb8dc, 0xff2679bc, 0xff2679bc, 0xff277cbe, 0xff287dbf, 0xff287dbf, 0xffffffff, 0xff7da0cb
,0xff7a9eca, 0xffffffff, 0xff277abd, 0xff277abd, 0xff277abd, 0xff277abd, 0xff2576ba, 0xff347fbf, 0xff347fbf, 0xff2576ba, 0xff277abd, 0xff277abd, 0xff277abd, 0xff277abd, 0xffffffff, 0xff7a9eca
,0xff769bc8, 0xffffffff, 0xff2577ba, 0xff2577ba, 0xff2577ba, 0xff2577ba, 0xff2373b7, 0xffd3e4f1, 0xffd3e4f1, 0xff2373b7, 0xff2577ba, 0xff2577ba, 0xff2577ba, 0xff2577ba, 0xffffffff, 0xff769bc8
,0xff7399c7, 0xffffffff, 0xff2474b8, 0xff2474b8, 0xff2474b8, 0xff2474b8, 0xff2270b5, 0xffd3e3f1, 0xffd3e3f1, 0xff2270b5, 0xff2474b8, 0xff2474b8, 0xff2474b8, 0xff2474b8, 0xffffffff, 0xff7399c7
,0xff7096c6, 0xffffffff, 0xff2372b7, 0xff2372b7, 0xff2372b7, 0xff2372b7, 0xff216fb4, 0xff216fb4, 0xff216fb4, 0xff216fb4, 0xff2372b7, 0xff2372b7, 0xff2372b7, 0xff2372b7, 0xffffffff, 0xff7096c6
,0xff6d94c4, 0xffffffff, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xff2270b5, 0xffffffff, 0xff6d94c4
,0xff6a92c3, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff6a92c3
,0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2, 0xff6890c2
};



void BitmapImage::initPlatformData()
{
	EA::WebKit::Parameters& params = EA::WebKit::GetParameters();
	m_source.m_gammaAndColorProfileOption = (params.mIgnoreGammaAndColorProfile) ? ImageSource::GammaAndColorProfileIgnored : ImageSource::GammaAndColorProfileApplied;
}

void BitmapImage::invalidatePlatformData()
{
}

PassRefPtr<Image> Image::loadPlatformResource(const char *name)
{
    RefPtr<BitmapImage> img = 0;

    const char* kMissingName="missingImage";                            // The name sent down by cachedimage.cpp when a broken image is found. 
    if (strcmp(name,kMissingName) == 0)
    {
       cairo_surface_t *pSurface = cairo_image_surface_create_for_data((unsigned char*)&brokenImageBin[0], CAIRO_FORMAT_ARGB32, kBrokenImageWidth, kBrokenImageHeight, kBrokenImageWidth * sizeof(uint32_t));
       img = BitmapImage::create(cairo_surface_reference(pSurface));    // BitmapImage will release the passed in surface on destruction.
       cairo_surface_destroy(pSurface);                                 // BitmapImage now holds the only ref.
    }
    else
    {
        img = BitmapImage::create();
    }
    return img.release();
}

#if 0
PassRefPtr<BitmapImage> BitmapImage::create(HBITMAP hBitmap)
{
    // Implement this function if we use plugin system.
    RefPtr<BitmapImage> img = BitmapImage::create();
    return img.release();
}
#endif
}

