/*
 * Copyright (C) 2010 Nokia Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc.  All rights reserved.
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(WEB_SOCKETS)
#include "SocketStreamHandle.h"

#include "KURL.h"
#include "Logging.h"
#include "NotImplemented.h"
#include "SocketStreamHandleClient.h"
#include "SocketStreamHandlePrivate.h"
#include <wtf/MainThread.h>

#include <internal/include/EAWebkitEASTLHelpers.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKit_p.h>

namespace WebCore {

SocketStreamHandlePrivate::SocketStreamHandlePrivate(SocketStreamHandle* streamHandle, const KURL& url)
: m_streamHandle(streamHandle)
, m_socketHandle(NULL)
{
	ASSERT(WTF::isMainThread());

	bool isSecure = url.protocolIs("wss");
	EA::WebKit::SocketType sockType = isSecure ? EA::WebKit::kSecuredTCP : EA::WebKit::kUnsecuredTCP;

	unsigned int port = url.hasPort() ? url.port() : (isSecure ? 443 : 80);
	EA::WebKit::FixedString8_128 hostUrl;
	EA::WebKit::ConvertToString8(url.host(),hostUrl);
	
	EA::WebKit::SocketTransportHandler* pSocketTxHandler = EA::WebKit::GetSocketTransportHandler();
	EAW_ASSERT_MSG(pSocketTxHandler, "Using web sockets without supplying a socket transport handler\n");
	if(pSocketTxHandler)
	{
		m_socketHandle = pSocketTxHandler->OpenSocket(hostUrl.c_str(), port, sockType, this);
		ASSERT(m_socketHandle);
	}
}

SocketStreamHandlePrivate::~SocketStreamHandlePrivate()
{
	ASSERT(m_socketHandle == NULL);
}

void SocketStreamHandlePrivate::RecvedData(const char8_t* data, int32_t length)
{
	if (m_streamHandle && m_streamHandle->client()) 
	{
		m_streamHandle->client()->didReceiveSocketStreamData(m_streamHandle, data, length);
	}
}
void SocketStreamHandlePrivate::SocketEvent(EA::WebKit::SocketEventType eventType)
{
	switch(eventType)
	{
	case EA::WebKit::kSocketConnected:
		if (m_streamHandle && m_streamHandle->client()) 
		{
			m_streamHandle->m_state = SocketStreamHandleBase::Open;
			m_streamHandle->client()->didOpenSocketStream(m_streamHandle);
		}
		break;
	case EA::WebKit::kSocketClose:
		// Following is ported from Qt. This code does not call SocketStreamHandle::platformClose() so we need to do that manually on the handler side.
		if (m_streamHandle && m_streamHandle->client()) 
		{
			SocketStreamHandle* streamHandle = m_streamHandle;
			m_streamHandle = 0;
			m_socketHandle = 0;
			// This following call deletes _this_. Nothing should be after it.
			streamHandle->client()->didCloseSocketStream(streamHandle);
		}
		break;
	case EA::WebKit::kSocketError:
		// Following is ported from Qt. This code does not call SocketStreamHandle::platformClose() so we need to do that manually on the handler side.
		if (m_streamHandle && m_streamHandle->client()) 
		{
			SocketStreamHandle* streamHandle = m_streamHandle;
			m_streamHandle = 0;
			m_socketHandle = 0;
			// This following call deletes _this_. Nothing should be after it.
			streamHandle->client()->didCloseSocketStream(streamHandle);
		}
		break;
	default:
		ASSERT(false);
		break;
	}
}

int32_t SocketStreamHandlePrivate::Send(const char8_t* data, int32_t length)
{
	if (!m_socketHandle)
		return 0;

	int32_t sentSize = 0;
	if(EA::WebKit::SocketTransportHandler* pSocketTxHandler = EA::WebKit::GetSocketTransportHandler())
	{
		sentSize = pSocketTxHandler->SendData(m_socketHandle,data, length);
	}
	return sentSize;
}

void SocketStreamHandlePrivate::Close()
{
	if (!m_socketHandle)
		return;
	
	if(EA::WebKit::SocketTransportHandler* pSocketTxHandler = EA::WebKit::GetSocketTransportHandler())
	{
		pSocketTxHandler->CloseSocket(m_socketHandle);
	}
	m_socketHandle = NULL;
}

SocketStreamHandle::SocketStreamHandle(const KURL& url, SocketStreamHandleClient* client)
    : SocketStreamHandleBase(url, client)
{
	ASSERT(WTF::isMainThread());
	LOG(Network, "SocketStreamHandle %p new client %p", this, m_client);
    m_p = new SocketStreamHandlePrivate(this, url);
}

SocketStreamHandle::~SocketStreamHandle()
{
	ASSERT(WTF::isMainThread());
	LOG(Network, "SocketStreamHandle %p delete", this);
    setClient(0);
    delete m_p;
}

int SocketStreamHandle::platformSend(const char* data, int len)
{
	ASSERT(WTF::isMainThread());
	LOG(Network, "SocketStreamHandle %p platformSend", this);
    return m_p->Send(data, len);
}

void SocketStreamHandle::platformClose()
{
	ASSERT(WTF::isMainThread());
	LOG(Network, "SocketStreamHandle %p platformClose", this);
    m_p->Close();
}

void SocketStreamHandle::didReceiveAuthenticationChallenge(const AuthenticationChallenge&)
{
	ASSERT(WTF::isMainThread());
	notImplemented();
}

void SocketStreamHandle::receivedCredential(const AuthenticationChallenge&, const Credential&)
{
	ASSERT(WTF::isMainThread());
	notImplemented();
}

void SocketStreamHandle::receivedRequestToContinueWithoutCredential(const AuthenticationChallenge&)
{
	ASSERT(WTF::isMainThread());
	notImplemented();
}

void SocketStreamHandle::receivedCancellation(const AuthenticationChallenge&)
{
	ASSERT(WTF::isMainThread());
	notImplemented();
}

} // namespace WebCore

#endif
