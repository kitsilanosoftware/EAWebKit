/*
 * Copyright (C) 2006 Lars Knoll <lars@trolltech.com>
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "TextBreakIterator.h"
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebkit_p.h>
#include <internal/include/EAWebKitAssert.h>

namespace WebCore {

	// The sole purpose of this class is to wrap our own type of break iterator.
	// We can possibly avoid this wrapper class if we go to the header file and:
	//     typedef EA::Text::BreakIteratorBase TextBreakIterator;
	// But let's wait until we do the other modifications listed below first.

	class TextBreakIterator
	{
	public:
		void* mpBreakIterator;
	};


	// Note by Paul Pedriana: WebKit accesses these iterators only 
	// ever one at a time, and not recursively or by multiple threads. The implication
	// is that we can have a single version of each. 
	
	// If EA::Text::BreakIteratorBase interface is updated to have toNextBoundary()/toPreviousBoundary(), 
	// following file level globals can become function level statics.

    static TextBreakIterator                gTextBreakIterator; // This points to one of the 4 iterator types at any given instance of time.

	TextBreakIterator* characterBreakIterator(const UChar* pText, int length)
    {
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        gTextBreakIterator.mpBreakIterator = pTS->CharacterBreakIterator((EA::WebKit::Char *) pText, length);    
        return &gTextBreakIterator;
    }

    TextBreakIterator* cursorMovementIterator(const UChar* string, int length)
	{
		return characterBreakIterator(string, length);
	}

	TextBreakIterator* wordBreakIterator(const UChar* pText, int length)
	{
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        gTextBreakIterator.mpBreakIterator = pTS->WordBreakIterator((EA::WebKit::Char *) pText, length);
        return &gTextBreakIterator;
    }

	TextBreakIterator* lineBreakIterator(const UChar* pText, int length)
	{
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        gTextBreakIterator.mpBreakIterator = pTS->LineBreakIterator((EA::WebKit::Char *) pText, length);
        return &gTextBreakIterator;
    }
    
	TextBreakIterator* sentenceBreakIterator(const UChar* pText, int length)
	{
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        gTextBreakIterator.mpBreakIterator = pTS->SentenceBreakIterator((EA::WebKit::Char *) pText, length);
        return &gTextBreakIterator;
    }

	int textBreakFirst(TextBreakIterator* pIterator)
	{
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->TextBreakFirst(pIterator->mpBreakIterator);   
    }
    
	int textBreakNext(TextBreakIterator* pIterator)
    {
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->TextBreakNext(pIterator->mpBreakIterator);
    }	
    
	int textBreakPrevious(TextBreakIterator* pIterator)
	{
		EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->TextBreakPrevious(pIterator->mpBreakIterator);
	}
	
    int textBreakCurrent(TextBreakIterator* pIterator)
	{
		EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
		return pTS->TextBreakCurrent(pIterator->mpBreakIterator);
	}

	int textBreakPreceding(TextBreakIterator* pIterator, int position)
    {
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->TextBreakPreceding(pIterator->mpBreakIterator, position);
    }

	int textBreakFollowing(TextBreakIterator* pIterator, int position)
	{
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->TextBreakFollowing(pIterator->mpBreakIterator, position);
    }

	bool isTextBreak(TextBreakIterator* pIterator, int position)
	{
		// If we are being asked about the most recent break position, then we know it is a break position.
		// Otherwise the only means we have to implementing this is to walk through the entire string
	    // and check each break against position.
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        return pTS->IsTextBreak(pIterator->mpBreakIterator, position);  
    }

    TextBreakIterator* acquireLineBreakIterator(const UChar* pText, int length, const AtomicString& locale)
    { 
        TextBreakIterator *newIterator = new TextBreakIterator();

        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        newIterator->mpBreakIterator = pTS->AcquireLineBreakIterator((EA::WebKit::Char *)pText, length);

        return newIterator;
    }

    void releaseLineBreakIterator(TextBreakIterator* pIterator)
    {
        EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
        pTS->ReleaseLineBreakIterator(pIterator->mpBreakIterator);

        delete pIterator;
    }
}
