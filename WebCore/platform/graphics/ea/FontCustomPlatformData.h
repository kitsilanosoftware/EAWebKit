/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#ifndef FontCustomPlatformData_h
#define FontCustomPlatformData_h

#include "FontOrientation.h"
#include "FontRenderingMode.h"
#include "TextOrientation.h"
#include "FontWidthVariant.h"
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <EAWebKit/EAWebKitTextInterface.h>

namespace WebCore {

class FontPlatformData;
class SharedBuffer;

// Note by Paul Pedriana: This is a class which manages the creation of 
// a FontPlatformData (more or less equivalent to EA::Text::Font) from
// a font file transferred over the Internet, as with the @font-face
// directive (dynamic downloadable fonts).

struct FontCustomPlatformData {
    WTF_MAKE_NONCOPYABLE(FontCustomPlatformData);
public:
    ~FontCustomPlatformData();


    // This setting is used to provide ways of switching between multiple rendering modes that 
    // may have different metrics. It is used to switch between CG and GDI text on Windows.
    // enum FontRenderingMode { NormalRenderingMode, AlternateRenderingMode };
    FontPlatformData fontPlatformData(int size, bool bold, bool italic, FontOrientation = Horizontal, TextOrientation = TextOrientationVerticalRight, FontWidthVariant = RegularWidth, FontRenderingMode = NormalRenderingMode);
    FontCustomPlatformData(SharedBuffer* buffer); 
    static bool supportsFormat(const String&);

private:
    SharedBuffer*       mpFontBuffer;
};

FontCustomPlatformData* createFontCustomPlatformData(SharedBuffer* buffer);

} // namespace WebCore

#endif // FontCustomPlatformData_h
