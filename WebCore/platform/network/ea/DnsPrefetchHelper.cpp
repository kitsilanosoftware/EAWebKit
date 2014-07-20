/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
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
#include "DnsPrefetchHelper.h"

#include "PlatformString.h"
#if 0 
#include <internal/include/EAWebkitEASTLHelpers.h>
#include "dirtysock.h"
#endif
namespace WebCore {
#if 0 
DnsPrefetchHelper::DnsPrefetchHelper(uint32_t maxConcurrentDNSPrefetchRequest /* = kDefaultMaxConcurrentDNSPrefetchRequest */)
: mMaxConcurrentDNSPrefetchRequest(maxConcurrentDNSPrefetchRequest)
{
}

void DnsPrefetchHelper::LookUp(const char8_t* hostName)
{
	if(mCurrentlyLookedUpHostList.size() < mMaxConcurrentDNSPrefetchRequest)
	{
		// Temp. stuff here. The SocketCreate code will be removed eventually when this code moves to DirtySDK.
		static bool socketCreated = false;
		if(!socketCreated)
		{
			SocketCreate(0);
			socketCreated = true;
		}
		HostentT* host = SocketLookup(hostName, 30);
		ASSERT(host);
		
		if(host)
		{
			mCurrentlyLookedUpHostList.push_back(host);
		}

	}
}

void DnsPrefetchHelper::Poll()
{
	HostList::iterator iter = mCurrentlyLookedUpHostList.begin();
	while(iter != mCurrentlyLookedUpHostList.end())
	{
		if((*iter)->Done(*iter))
		{
			(*iter)->Free(*iter);
			iter = mCurrentlyLookedUpHostList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
#endif
// this is called on mouse over a href and on page loading
void prefetchDNS(const String& hostname)
{
//This code works but is unfinished. Need to move this in the DirtySDK transport handler.    
#if 0
	if (!hostname.isEmpty())
	{
        static DnsPrefetchHelper dnsPrefetchHelper;
		
		EA::WebKit::FixedString8_256 hostname8; 
		EA::WebKit::ConvertToString8(hostname, hostname8);
		
		dnsPrefetchHelper.LookUp(hostname8.c_str());//fix
		dnsPrefetchHelper.Poll();
		
    }
	#endif
}

}
