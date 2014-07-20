/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * 
 * All rights reserved.
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
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
#include "ResourceHandle.h"

#include "ChromeClientEA.h"
#include "CachedResourceLoader.h"
#include "Frame.h"
#include "FrameNetworkingContext.h"
#include "FrameLoaderClientEA.h"
#include "NotImplemented.h"
#include "Page.h"
#include "ResourceHandleClient.h"
#include "ResourceHandleInternal.h"
#include "SharedBuffer.h"

#include "ResourceHandleManagerEA.h"



namespace WebCore {

class WebCoreSynchronousLoader : public ResourceHandleClient {
public:
    WebCoreSynchronousLoader();


    virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&);
    virtual void didReceiveData(ResourceHandle*, const char*, int, int lengthReceived);
    virtual void didFinishLoading(ResourceHandle*, double /*finishTime*/);
    virtual void didFail(ResourceHandle*, const ResourceError&);

    ResourceResponse resourceResponse() const { return m_response; }
    ResourceError resourceError() const { return m_error; }
    Vector<char> data() const { return m_data; }

private:
    ResourceResponse m_response;
    ResourceError m_error;
    Vector<char> m_data;
};

WebCoreSynchronousLoader::WebCoreSynchronousLoader()
{
}

void WebCoreSynchronousLoader::didReceiveResponse(ResourceHandle*, const ResourceResponse& response)
{
    m_response = response;
}

void WebCoreSynchronousLoader::didReceiveData(ResourceHandle*, const char* data, int length, int)
{
    m_data.append(data, length);
}

void WebCoreSynchronousLoader::didFinishLoading(ResourceHandle*, double)
{
}

void WebCoreSynchronousLoader::didFail(ResourceHandle*, const ResourceError& error)
{
    m_error = error;
}


ResourceHandleInternal::~ResourceHandleInternal()
{
}

ResourceHandle::~ResourceHandle()
{
	cancel();
}

// Note by Arpit Baldeva: NetworkingContext comes from platform specific implementation(It used to be WebCore::Frame in our last port). It makes it easy to tag some information to every handle. For example,
// Qt uses it to have a different NetworkAccessManager for each View.
// A detailed discussion can be found at - http://www.mail-archive.com/webkit-dev@lists.webkit.org/msg12786.html 

bool ResourceHandle::start(NetworkingContext* context)
{
    // If NetworkingContext is invalid then we are no longer attached to a Page,
    // this must be an attempted load from an unload event handler, so let's just block it.
	if (context && !context->isValid())
        return false;

    // Note by Arpit Baldeva: For some reason, ResourceHandleInternal removes the user name/passwd from the url. Qt port stuffs it back in the URL. 
	// CURL does not. So we don't.
	// Update: The reason Qt stuffs it in is possibly because its network layer requires it.
	
	// The context can be helpful to figure out some top-level info inside the ResourceHandleManager. For example, the View this ResourceHandle 
	// belongs to.
	// Part of the platform specific functionality now lives in the ResourceHandleInternal.
	getInternal()->m_context = context;
	
	ResourceHandleManager::sharedInstance()->add(this);

	return true;
}

void ResourceHandle::cancel()
{
	ResourceHandleManager::sharedInstance()->cancel(this);
}

bool ResourceHandle::loadsBlocked()
{
	notImplemented();
	return false;
}

bool ResourceHandle::willLoadFromCache(ResourceRequest& request, Frame* frame)
{
	notImplemented();
	return false;
}

void ResourceHandle::loadResourceSynchronously(NetworkingContext* context, const ResourceRequest& request, StoredCredentials /*storedCredentials*/, ResourceError& error, ResourceResponse& response, Vector<char>& data)
{
	WebCoreSynchronousLoader syncLoader;
	RefPtr<ResourceHandle> handle = adoptRef(new ResourceHandle(request, &syncLoader, true, false));
	handle->getInternal()->m_context = context;

	ResourceHandleManager* manager = ResourceHandleManager::sharedInstance();

	manager->loadResourceSynchronously(handle.get());

	error = syncLoader.resourceError();
	data = syncLoader.data();
	response = syncLoader.resourceResponse();
}

void ResourceHandle::platformSetDefersLoading(bool defers)
{
	// Pause is a better name than defers. You can think of it as pausing or resuming a download.
	ResourceHandleManager::sharedInstance()->changeResourceHandleState(this, defers);
}

} // namespace WebCore
