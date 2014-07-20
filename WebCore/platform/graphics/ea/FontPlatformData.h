/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
    Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
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
#ifndef FontPlatformData_h
#define FontPlatformData_h

#include <wtf/Forward.h>
#include "FontDescription.h"
#include "FontOrientation.h"
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebkitAssert.h>

namespace WebCore {

class FontPlatformData;
class SharedBuffer;

class FontPlatformDataPrivate {
    WTF_MAKE_NONCOPYABLE(FontPlatformDataPrivate);
public:
    FontPlatformDataPrivate()
        : refCount(1)
        , mpFont(0)
        , size(0.0f)    
        , bold(false)
        , oblique(false)
    {}
    FontPlatformDataPrivate(const float size, const bool bold, const bool oblique)
        : refCount(1)
        , mpFont(0)
        , size(size)
        , bold(bold)
        , oblique(oblique)
    {}
	~FontPlatformDataPrivate();
private:
	friend class FontPlatformData;

	unsigned refCount;
    EA::WebKit::IFont* mpFont;
    float size;
    bool bold : 1;
    bool oblique : 1;
};

class FontPlatformData {
    WTF_MAKE_FAST_ALLOCATED(FontPlatformData);
public:
    FontPlatformData(float size, bool bold, bool oblique);
    FontPlatformData(const FontPlatformData &);
    FontPlatformData(const FontDescription&, const AtomicString& familyName, const UChar* characters, int length = 0, int wordSpacing = 0, int letterSpacing = 0);
    FontPlatformData(SharedBuffer* buffer, int size, bool bold, bool italic);  
   
    FontPlatformData(WTF::HashTableDeletedValueType)
        : m_data(reinterpret_cast<FontPlatformDataPrivate*>(-1))
    {}

    ~FontPlatformData();

    FontPlatformData& operator=(const FontPlatformData&);
    bool operator==(const FontPlatformData&) const;

    bool isHashTableDeletedValue() const
    {
        return m_data == reinterpret_cast<FontPlatformDataPrivate*>(-1);
    }

    EA::WebKit::IFont* font() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data)
            return m_data->mpFont;
        return 0;
    }
    float size() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data)
            return m_data->size;
        return 0.0f;
    }

    bool bold() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data)
            return m_data->bold;
        return false;
    }
    bool italic() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data && m_data->mpFont)
        {
            return m_data->mpFont->IsItalic();
        }
        return false;
    }
    bool smallCaps() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data && m_data->mpFont)
        {
            return m_data->mpFont->IsSmallCaps();
        }    
        return false;
    }

    int pixelSize() const
    {
        EAW_ASSERT(m_data != reinterpret_cast<FontPlatformDataPrivate*>(-1));
        if (m_data && m_data->mpFont)
        {
            return (int) m_data->mpFont->GetSize();
        }    
        return 0;
    }
    
    bool isFixedPitch()
    {
        if (m_data && m_data->mpFont)
        {
            EA::WebKit::FontMetrics fontMetrics;
            m_data->mpFont->GetFontMetrics(fontMetrics);
            return fontMetrics.mPitch == EA::WebKit::kPitchFixed;
        }
        return false;
    }

    FontOrientation orientation() const { return Horizontal; } // FIXME: Implement.
    void setOrientation(FontOrientation orientation) {} // FIXME: Implement.

    unsigned hash() const;

#ifndef NDEBUG
    String description() const;
#endif
private:
    FontPlatformDataPrivate* m_data;
};

} // namespace WebCore

#endif // FontPlatformData_h
