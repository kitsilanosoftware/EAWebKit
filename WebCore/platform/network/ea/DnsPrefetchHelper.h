/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
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
#ifndef DnsPrefetchHelper_h
#define DnsPrefetchHelper_h

#include <EABase/eabase.h>
#include <eastl/fixed_list.h>
#include <EAWebKit/EAWebkitAllocator.h>

struct HostentT;
namespace WebCore 
{
	const uint32_t kDefaultMaxConcurrentDNSPrefetchRequest = 8;
	class DnsPrefetchHelper
	{
	public:
		explicit DnsPrefetchHelper(uint32_t maxConcurrentDNSPrefetchRequest = kDefaultMaxConcurrentDNSPrefetchRequest);
		void LookUp(const char8_t* hostName);
		void Poll();
	private:
		int mMaxConcurrentDNSPrefetchRequest;
		
		typedef eastl::fixed_list<HostentT*, kDefaultMaxConcurrentDNSPrefetchRequest, true, EA::WebKit::EASTLAllocator> HostList;
		HostList mCurrentlyLookedUpHostList;
	};
}

#endif // DnsPrefetchHelper_h
