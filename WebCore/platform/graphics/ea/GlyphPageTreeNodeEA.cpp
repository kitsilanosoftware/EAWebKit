/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
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
#include "GlyphPageTreeNode.h"
#include "SimpleFontData.h"
#include <EAWebKit/EAWebKitTextInterface.h>

namespace WebCore {


// This function returns true if the SimpleFontData has any of the glyphs
// associated with buffer/bufferLength.
bool GlyphPage::fill(unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength, const SimpleFontData* pSimpleFontData)
{
    bool haveGlyphs = false;

   // Is this always true?
    if (bufferLength <= GlyphPage::size)
    {
        EA::WebKit::IFont* const pFont = pSimpleFontData->getEAFont();

        if (pFont)
        {
            for (unsigned i = 0; i < bufferLength; i++)
            {
                EA::WebKit::GlyphId glyphId = EA::WebKit::kGlyphIdInvalid;

                pFont->GetGlyphIds(&buffer[i], 1, &glyphId, false);

                if (glyphId != EA::WebKit::kGlyphIdInvalid)
                {
                    setGlyphDataForIndex(i, glyphId, pSimpleFontData);
                    haveGlyphs = true;
                }
                else
                    setGlyphDataForIndex(i, 0, 0);
            }
        }
    }
    return haveGlyphs;
}
}
