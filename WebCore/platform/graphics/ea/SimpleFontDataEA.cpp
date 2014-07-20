/*
    Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
	Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.

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
#include "SimpleFontData.h"

#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebKitAssert.h>

namespace WebCore {

void SimpleFontData::platformInit()
{
    EA::WebKit::IFont* pFont = m_platformData.font();
    if(!pFont)
    {
        EAW_ASSERT(pFont);    
    }
    else
    {
        EA::WebKit::FontMetrics fontMetrics;
        pFont->GetFontMetrics(fontMetrics);

        // When leading is specified in a font it needs to be account for in the font ascent value.
        // We account for this by using the baseline direct since it is = ascent + leading.
        m_fontMetrics.setAscent(ceilf(fontMetrics.mfBaseline));
        m_fontMetrics.setDescent(ceilf(-fontMetrics.mfDescent));    // EAText uses a negative descent. 
        
        // This fix was contributed by YC Chin.  The Line spacing and gap calculations were wrong and did not include leading.
        m_fontMetrics.setLineGap(fontMetrics.mfLeading);        
        m_fontMetrics.setLineSpacing(fontMetrics.mfLineHeight);

        m_fontMetrics.setUnitsPerEm(1000);                          // This is a typical value for a TrueType font, not necessarily the actual value.
        m_fontMetrics.setXHeight(fontMetrics.mfXHeight);            // This is the height of lower case characters.


        // Load in a space character to get the width metrics for it.
        const EA::WebKit::Char  cSpace  = ' ';
        EA::WebKit::GlyphId  glyphId = 0;
        pFont->GetGlyphIds(&cSpace, 1, &glyphId, true);

        EA::WebKit::GlyphMetrics glyphMetrics;
        pFont->GetGlyphMetrics(glyphId, glyphMetrics);
        m_spaceWidth = glyphMetrics.mfHAdvanceX;
    }
}

void SimpleFontData::platformDestroy()
{
    m_derivedFontData.clear();
}

void SimpleFontData::platformCharWidthInit()
{
    m_avgCharWidth = 0.f;       
    m_maxCharWidth = 0.f;
    initCharWidths();       // Use the default width estimator since EAText metrics does not seem to have it.
}

FloatRect SimpleFontData::platformBoundsForGlyph(Glyph) const
{
    // Seems like we could get this info but some other platforms seem to just return a empty rect too. 
    return FloatRect();
}

SimpleFontData* SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
    {
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    }

    if (!m_derivedFontData->smallCaps)
    {
        // Note by Paul Pedriana:
        // We simply create a version of this font that is 70% of its current size.
        // Question: Is there a means to get a more proper small caps variant?
        // Actually I think we just need to modify the code below to get a true small
        // caps font, but shouldn't desc.m_smallCaps be set to true in such a case?
        // and if m_smallCaps is true then we don't need 70% size.

        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f * fontDescription.computedSize());

        // Note by Paul Pedriana: This using of ' ' is wrong. What we need to do is somehow find out what
        // characters the caller is expecting this font to support, but currently webkit doesn't provide
        // that information. It doesn't provide it likely because Windows and Macintosh have font systems
        // that fake support for characters not in a font by silently substituting other fonts. A result
        // of our situation here is that if small caps are requested for Japanese glyphs, we probably not
        // get a Japanese font.

         UChar cSpace = ' ';
        const AtomicString& familyName = desc.family().family();
        const FontPlatformData pfontPlatformData(desc, familyName, &cSpace, 1);        
        m_derivedFontData->smallCaps = adoptPtr(new SimpleFontData(pfontPlatformData));

    }

    return m_derivedFontData->smallCaps.get();
}

bool SimpleFontData::containsCharacters(const UChar* characters, int length) const
{
    int i = 0;
    EA::WebKit::IFont* const pFont =  m_platformData.font();
    EAW_ASSERT(pFont);

    if(pFont)
    {
        while(pFont->IsCharSupported(characters[i]))
            ++i;
    }
    return (i == length);
}

void SimpleFontData::determinePitch()
{
     m_treatAsFixedPitch = m_platformData.isFixedPitch();
}

float SimpleFontData::platformWidthForGlyph(Glyph glyphId) const
{
    EA::WebKit::IFont* const pFont = getEAFont();
    EAW_ASSERT(pFont);

    EA::WebKit::GlyphMetrics glyphMetrics;
    glyphMetrics.mfHAdvanceX = m_spaceWidth; // Default value.
    if(pFont)
		pFont->GetGlyphMetrics(glyphId, glyphMetrics);

    return glyphMetrics.mfHAdvanceX;
}

SimpleFontData* SimpleFontData::emphasisMarkFontData(const FontDescription&) const
{
    // EAWebkitTODO
    return NULL;
}

} // WebCore