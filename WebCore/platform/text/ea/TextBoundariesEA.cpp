/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * All rights reserved.
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

#include "TextBoundaries.h"

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebkit_p.h>

namespace WebCore {

int findNextWordFromIndex(UChar const* buffer, int len, int position, bool forward)
{
	// Currently, this function does not execute correctly. So if a user wants to move from word to word, he'd end up moving to 
	// end of the word 1 -> start of word 2-> end of word 2 -> start of word 3 (......) 
	// Adjust the example below after the update.
	/// The arrows below show the positions that would be returned by 
	/// successive calls to GetNextWordBreak with the default wordBreakType.
	///    "Hello   world   hello   world"
	///     ^    ^  ^    ^  ^    ^  ^    ^
    EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
    return pTS->FindNextWordFromIndex((EA::WebKit::Char*) buffer, len, position, forward);

}

void findWordBoundary(UChar const* buffer, int len, int position, int* start, int* end)
{
	// A simple test for this function is to double click a word in rendered text and it should end up being selected.
	// This function executes correctly. However, once the fix mentioned above is put in place, verify that it does not break this functionality.
    EA::WebKit::ITextSystem* pTS = EA::WebKit::GetTextSystem();
    pTS->FindWordBoundary((EA::WebKit::Char*) buffer, len, position, start, end);
}

}

