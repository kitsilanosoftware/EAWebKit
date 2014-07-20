/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
    Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
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
#include "config.h"
#include "FontCache.h"

#include "FontDescription.h"
#include "FontPlatformData.h"
#include "Font.h"
#include "PlatformString.h"
#include <utility>
#include <wtf/ListHashSet.h>
#include <wtf/StdLibExtras.h>
#include <wtf/text/StringHash.h>

#include <internal/include/EAWebKit_p.h>
#include <EAWebkit/EAWebKitTextInterface.h>

using namespace WTF;

namespace WebCore {


void FontCache::platformInit()
{
}

const SimpleFontData* FontCache::getFontDataForCharacters(const Font& font, const UChar* chars, int length)
{
    // The comments below are valid but we still need to support fallback fonts for sites out of our control (like a payment site for example).
    // So we need to send down a sample glyph with the fontDescription and prefered family name so that EAText can find an alternate font. 
    // For example, several sites just use Times New Roman expecting that it will include Chinese fonts but our Time New Roman does not have it so
    // it drops back in here as a last resort.
    // Ideally, it would be great if we could pass down a sample glyph to EAText when we first get a font instead of just machting the family name/description.
    // Seems that other text implementations might be able to automatically replace a missing glyph with another font so they just return NULL here.
    

    // It may be better to have a leak free implementation here.  See the commented out code below as example however that implementation does not pass 
    // down a sample char so it is unable to confirm that the glyph is actually supported by the font.


    const FontDescription& fontDescription(font.fontDescription());
    const AtomicString&    fontFamily(font.family().family());
    FontPlatformData       fontPlatformData(fontDescription, fontFamily, chars, length);
    return new SimpleFontData(fontPlatformData);
/*    
    // Note by Arpit Baldeva: 
	// This method is called when WebCore can not find the glyphs for the characters in the loaded font. This method is intended
	// to serve as a "fallback to available fonts in the system". Since we never want to load fonts ourselves(the application is
	// supposed to preload the fonts it needs), we simply return 0 here.
#if 1
	return 0;
#else

	// In case, we need to implement this functionality, the leak free implementation is as follows. Following makes sure that the
	// created font ends up in a list that is freed at the Shutdown.

    const FontDescription& fontDescription(font.fontDescription());
    const AtomicString&    fontFamily(font.family().family());
    
	FontPlatformData* platformData = getCachedFontPlatformData(font.fontDescription(),  fontFamily, false); 
	SimpleFontData* fontData = getCachedFontData(platformData);
	// ASSERT(fontData->containsCharacters(characters, length));
	return fontData;
#endif
*/
    
}

SimpleFontData* FontCache::getSimilarFontPlatformData(const Font& font)
{
	// See the comments in the FontCache::getFontDataForCharacters. This function serves similar purpose.
#if 1
	return 0;
#else
	const FontDescription& fontDescription(font.fontDescription());
	const AtomicString&    fontFamily(font.family().family());

	FontPlatformData* platformData = getCachedFontPlatformData(font.fontDescription(),  fontFamily, false); 
	SimpleFontData* fontData = getCachedFontData(platformData);
	return fontData;
#endif
}

SimpleFontData* FontCache::getLastResortFallbackFont(const FontDescription& fontDescription, ShouldRetain)
{
	// We do want this method to be enabled in case everything else fails and the application has loaded a fallback font.

	// As a last resort, we create the font out of the standard font specified by the application in the EA::WebKit::Parameters
	// when initializing the library.

	const EA::WebKit::Parameters& params = EA::WebKit::GetParameters();
	const char16_t* type = params.mFontFamilyStandard;
	AtomicString fontFamily(type);

	FontPlatformData* platformData = getCachedFontPlatformData(fontDescription,  fontFamily, false); 
	SimpleFontData* fontData = getCachedFontData(platformData);
	return fontData;
}

void FontCache::getTraitsInFamily(const AtomicString&, Vector<unsigned>&)
{
}

FontPlatformData* FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
{
   // To consider: We should examine if it is possible to send down a sample char when initially creating a font.  
   // Currently we just send down an empty const UChar cSpace(' ') when no sample char is available.

    const UChar cSpace(' ');  
    return new FontPlatformData(fontDescription, familyName, &cSpace, 1);
}

} // namespace WebCore

