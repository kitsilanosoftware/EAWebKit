/*
 * Copyright (C) 2004, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 * Copyright (C) 2012 Electronic Arts, Inc. All rights reserved.
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
#include "TextCodecEA.h"
#include "PlatformString.h"
#include <wtf/text/CString.h>
#include <wtf/text/StringBuffer.h>

#include <EAIO/FnEncode.h>
namespace WebCore {

static PassOwnPtr<TextCodec> newTextCodecEA(const TextEncoding& encoding, const void*)
{
	return adoptPtr(new TextCodecEA(encoding));
}

void TextCodecEA::registerEncodingNames(EncodingNameRegistrar registrar)
{
	//Base
	registrar("UTF-8", "UTF-8");
	//Extended
	registrar("windows-65001", "UTF-8");
	registrar("unicode11utf8", "UTF-8");
	registrar("unicode20utf8", "UTF-8");
	registrar("x-unicode20utf8", "UTF-8");
	registrar("utf8", "UTF-8");

}

void TextCodecEA::registerCodecs(TextCodecRegistrar registrar)
{
	registrar("UTF-8", newTextCodecEA, 0);
}

TextCodecEA::TextCodecEA(const TextEncoding& encoding)
: m_encoding(encoding)
, m_numBufferedBytes(0)
, m_expectedSize(0)
{
// #if _DEBUG
// 	memset(m_bufferedBytes, 0, sizeof(m_bufferedBytes));
// #endif
}

TextCodecEA::~TextCodecEA()
{
}


// This function can be called multiple times successively, with flush being false for
// each time but the last. It's possible that the data may be such that a multibyte
// character straddles the division between two blocks given in successive calls.
// So we must save a little buffer of m_bufferedBytes to deal with this.
// In our implementation here, we efficiently solve UTF-8 decoding without allocating
// memory in most cases, including the case whereby we recieve incomplete multi-byte
// sequences.

//Note by Arpit Baldeva: Use the string buffer for decoding instead of the conditional stack based. This is simply because it did not save us any allocation.
//After decoding, we end up appending to a String which does the allocation we avoided. Worst case, we could be decoding a 90K js/css in 1 shot(with the
//cached script/css mechanism) and end up allocating 180K (90K * sizeof(UChar))buffer twice. This is because we free the decode buffer "after" appending the data
//to the string we return. So our peak memory usage would be doubled.
//Coincidentally, after doing this change, I realized that UTF16 text codec also uses a StringBuffer.
#define USE_STRING_BUFFER_FOR_DECODING 1

String TextCodecEA::decode(const char* bytes, size_t length, bool flush, bool /*stopOnError*/, bool& sawError)
{
	String resultString;

	// To consider: support 'sawError'.
	sawError = false;

	// ASSERT(m_encoding == "UTF-8");

	if(length)
	{
		if(m_numBufferedBytes) // If we have any leftover bytes from the last call...
		{
			const size_t additionalRequiredBytes = (m_expectedSize - m_numBufferedBytes);

			if(length >= additionalRequiredBytes) // If we now have enough bytes to finish the current UTF8 char... (in the large majority of cases this expression will be true, as additionalRequiredBytes is usually only 1 or 2)
			{
				// We build up a little multi-byte sequence for just this char and call ourselves recursively.
				char tempBuffer[kBufferSize];

				memcpy(tempBuffer, m_bufferedBytes, m_numBufferedBytes);
				memcpy(tempBuffer + m_numBufferedBytes, bytes, additionalRequiredBytes);

				m_numBufferedBytes = 0;
				bytes  += additionalRequiredBytes;
				length -= additionalRequiredBytes;

				// This should always result in exactly one decoded char with no leftover bytes.
				resultString = decode(tempBuffer, m_expectedSize, false, false, sawError);
				ASSERT((resultString.length() == 1) && (m_numBufferedBytes == 0));
			}
			else if((m_numBufferedBytes + length) < (size_t)kBufferSize)  // If we haven't gone over our buffer... (this expression should always be true)
			{
				// This pathway can execute if we have, for example, 1 buffered byte, 
				// are expecting 2 more, but length is only 1. This is unlikely but 
				// within the specifications of this decode API.
				memcpy(m_bufferedBytes + m_numBufferedBytes, bytes, length);
				m_numBufferedBytes += length;
				length = 0;//+ Note by Arpit Baldeva - We have buffered everything.
			}
			else // Else our buffer was not large enough to hold the multi-byte sequence. Currently this should not be possible based on our logic below.
			{
				m_numBufferedBytes = 0; // We panic and boot the buffer.
			}
		}
#if USE_STRING_BUFFER_FOR_DECODING
		//This is possible due to recursive nature of this function. Fortunately, we would only see 1 level of recursion.
		int existingLength = resultString.length();
		StringBuffer resultBuffer(length+existingLength);
		if(existingLength)
		{
			memcpy(resultBuffer.characters(),resultString.characters(),resultString.length()*sizeof(UChar));//copy the decoded string
		}
		UChar* pUsedBuffer = resultBuffer.characters() + existingLength; //offset the buffer
#else
		// We use stack space for the encoding where possible.
#if defined(_WIN32) || defined(EA_PLATFORM_UNIX)
		const size_t kMaxStackSize = 4096;
#else
		const size_t kMaxStackSize = 1024;
#endif
		UChar*       pMallocBuffer = NULL;
		UChar*       pUsedBuffer   = ((length * sizeof(UChar)) < kMaxStackSize) ? (UChar*)alloca(length * sizeof(UChar)) : (pMallocBuffer = (UChar*)fastMalloc(length * sizeof(UChar)));
#endif 
		size_t       j = 0;

		for(size_t i = 0; i<length; ++i, ++j)
		{
			if (((unsigned char)bytes[i] & 240) == 240)  // If we appear to have a 4-byte UTF8 sequence...
			{
				// We need to validate if (i + n) is within length and if not then copy it to m_bufferedBytes.
				if((i + 3) < length)  // If the expected length is within the actual length...
				{
					if(((unsigned char)bytes[i+1] & 128) && ((unsigned char)bytes[i+2] & 128) && ((unsigned char)bytes[i+3] & 128))
						pUsedBuffer[j] = ((unsigned char)bytes[i] - 240) * 65536 + ((unsigned char)bytes[i+1] - 128) * 4096 + ((unsigned char)bytes[i+2] - 128) * 64 + ((unsigned char)bytes[i+3] - 128);
					else
					{
						sawError = true;
						flush    = true;
					}
				}
				else  // Else the actual length isn't long enough.
				{
					m_expectedSize = 4;
					m_numBufferedBytes += (length - i);
					memcpy(m_bufferedBytes, bytes + i, length - i);
					break;
				}

				i += 3;
			}
			else if ( ((unsigned char)bytes[i]   & 224) == 224 )
			{
				if((i + 2) < length)
					if(((unsigned char)bytes[i+1] & 128) && ((unsigned char)bytes[i+2] & 128))
						pUsedBuffer[j] = ((unsigned char)bytes[i] - 224) * 4096 + ((unsigned char)bytes[i+1] - 128) * 64 + ((unsigned char)bytes[i+2] - 128);
					else
					{
						sawError = true;
						flush    = true;
					}
				else
				{
					m_expectedSize = 3;
					m_numBufferedBytes += (length - i);
					memcpy(m_bufferedBytes, bytes + i, length - i);
					break;
				}

				i += 2;
			}
			else if ( ((unsigned char)bytes[i]   & 192) == 192) 
			{
				if((i + 1) < length)
				{
					if((unsigned char)bytes[i+1] & 128)
						pUsedBuffer[j] = ((unsigned char)bytes[i] - 192) * 64 + ((unsigned char)bytes[i+1] - 128);
					else
					{
						sawError = true;
						flush    = true;
					}
				}
				else
				{
					m_expectedSize = 2;
					m_bufferedBytes[m_numBufferedBytes++] = bytes[i];
					break;
				}

				i++;
			}
			else if (((unsigned char)bytes[i] & 128) == 128)
				pUsedBuffer[j] = (unsigned char)bytes[i] & 127;
			else
				pUsedBuffer[j] = (unsigned char)bytes[i];
		}
#if USE_STRING_BUFFER_FOR_DECODING
		if(flush) // If flush is true, then this is the last call, so we unilaterally clear our buffer.
			m_numBufferedBytes = 0;

		resultBuffer.resize(j+existingLength);//Make sure we don't overwrite the leftover. Resize is pretty efficient as it simply adjusts length when going down in size as is the case here.
		return String::adopt(resultBuffer);
#else		
		resultString.append(pUsedBuffer, j);
		if(pMallocBuffer)
			fastFree(pMallocBuffer);
#endif	


	}

	if(flush) // If flush is true, then this is the last call, so we unilaterally clear our buffer.
		m_numBufferedBytes = 0;

	return resultString;
}


CString TextCodecEA::encode(const UChar* characters, size_t length, UnencodableHandling /*handling*/)
{
	CString result;

	if (length)
	{
#if defined(_WIN32) || defined(EA_PLATFORM_UNIX)
		const size_t kMaxStackSize = 4096;
#else
		const size_t kMaxStackSize = 1024;
#endif
		char* pMallocBuffer = NULL;

		size_t destLen = (EA::IO::StrlcpyUTF16ToUTF8(NULL, 0, characters, length))+1; // Call with NULL in order to get the required strlen., //+1 because a null character is appended by the function at the end.
		char* pUsedBuffer   = (destLen < kMaxStackSize) ? (char*)alloca(destLen) : (pMallocBuffer = (char*)fastMalloc(destLen));
		EA::IO::StrlcpyUTF16ToUTF8(pUsedBuffer, destLen, characters, length);

		result = CString(pUsedBuffer, destLen-1);//subtract the NULL character, string would append it.

		if(pMallocBuffer)
			fastFree(pMallocBuffer);
	}

	return result;
}
} // namespace WebCore
