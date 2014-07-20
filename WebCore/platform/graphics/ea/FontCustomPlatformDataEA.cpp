/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "SharedBuffer.h"
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebkit_p.h>
#include "WOFFFileFormat.h"

namespace WebCore {

FontCustomPlatformData::~FontCustomPlatformData()
{
   if (mpFontBuffer)
    {
        mpFontBuffer->deref();
        mpFontBuffer = NULL;
    }
}

FontCustomPlatformData::FontCustomPlatformData(SharedBuffer* buffer)  
: mpFontBuffer(buffer)
{
	if(mpFontBuffer)
        mpFontBuffer->ref();
}

FontPlatformData FontCustomPlatformData::fontPlatformData(int size, bool bold, bool italic, FontOrientation, TextOrientation, FontWidthVariant, FontRenderingMode)
{
	return FontPlatformData(mpFontBuffer, size, bold, italic);
}

FontCustomPlatformData* createFontCustomPlatformData(SharedBuffer* buffer)
{
    ASSERT_ARG(buffer, buffer);
    FontCustomPlatformData* pFontCustomPlatformData = NULL;

    // Check if we are dealing with a compressed WOFF font format.
    if (isWOFF(buffer))
    {
        Vector<char> sfnt;
        convertWOFFToSfnt(buffer, sfnt);
        if (!sfnt.size())
            return NULL;
        PassRefPtr<SharedBuffer> bufferWOFF = SharedBuffer::adoptVector(sfnt);
        
        EA::WebKit::IFont* pFont = EA::WebKit::GetTextSystem()->CreateNewFont(EA::WebKit::kFontTypeOutline, bufferWOFF->data(),(uint32_t)bufferWOFF->size());
        if (!pFont)
            return NULL;
        pFont->Release(); 

        pFontCustomPlatformData = new FontCustomPlatformData(bufferWOFF.get());
    }
    else
    {
        // We need to test here if the text system will accept the font format now so that we can return NULL on fail so that an alternate font can be used.
        EA::WebKit::IFont* pFont = EA::WebKit::GetTextSystem()->CreateNewFont(EA::WebKit::kFontTypeOutline, buffer->data(),(uint32_t)buffer->size());
        if (!pFont)
            return NULL;
        pFont->Release(); 

        pFontCustomPlatformData = new FontCustomPlatformData(buffer);
    }

    return pFontCustomPlatformData;
}


bool FontCustomPlatformData::supportsFormat(const String& format)
{
    if(equalIgnoringCase(format, "woff"))
        // If a page explicitly sets the font format to WOFF (e.g. format('woff')), we need to accept it so that the fontface can be considered for use.
        // A WOOF can be a truetype or opentype font but we don't need to verify this here since we will check if the font data can be created/supported in createFontCustomPlatformData(). 
        return true;

	if(EA::WebKit::ITextSystem* pTextSystem = EA::WebKit::GetTextSystem())
	{
		if(equalIgnoringCase(format, "truetype"))
			return pTextSystem->SupportsFormat(EA::WebKit::kTrueType);
		if(equalIgnoringCase(format, "opentype"))
			return pTextSystem->SupportsFormat(EA::WebKit::kOpenType);
	}
	return false;
}

}
