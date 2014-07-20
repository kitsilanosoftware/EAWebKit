/*
 * Copyright (C) 2004, 2005, 2006 Apple Inc.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.
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

#if defined(HAVE_CONFIG_H) && HAVE_CONFIG_H
#ifdef BUILDING_WITH_CMAKE
#include "cmakeconfig.h"
#else
#include "autotoolsconfig.h"
#endif
#endif

#include <wtf/Platform.h>

//+EAWebKitChange
//10/19/2011 - Added !PLATFORM(EA)
#if OS(WINDOWS) && !OS(WINCE) && !PLATFORM(QT) && !PLATFORM(CHROMIUM) && !PLATFORM(GTK) && !PLATFORM(WX) && !PLATFORM(EA)
#include <WebCore/WebCoreHeaderDetection.h>
#endif
//-EAWebKitChange

/* See note in wtf/Platform.h for more info on EXPORT_MACROS. */
#if USE(EXPORT_MACROS)

#include <wtf/ExportMacros.h>

#if defined(BUILDING_JavaScriptCore) || defined(BUILDING_WTF)
#define WTF_EXPORT_PRIVATE WTF_EXPORT
#define JS_EXPORT_PRIVATE WTF_EXPORT
#else
#define WTF_EXPORT_PRIVATE WTF_IMPORT
#define JS_EXPORT_PRIVATE WTF_IMPORT
#endif

#define JS_EXPORTDATA JS_EXPORT_PRIVATE
#define JS_EXPORTCLASS JS_EXPORT_PRIVATE

#if defined(BUILDING_WebCore) || defined(BUILDING_WebKit)
#define WEBKIT_EXPORTDATA WTF_EXPORT
#else
#define WEBKIT_EXPORTDATA WTF_IMPORT
#endif

#else /* !USE(EXPORT_MACROS) */

//+EAWebKitChange
//10/17/2011
#if !PLATFORM(CHROMIUM) && OS(WINDOWS) && !defined(BUILDING_WX__) && !COMPILER(GCC) && !PLATFORM(EA)
#if defined(BUILDING_JavaScriptCore) || defined(BUILDING_WTF)
#define JS_EXPORTDATA __declspec(dllexport)
#else
#define JS_EXPORTDATA __declspec(dllimport)
#endif
#if defined(BUILDING_WebCore) || defined(BUILDING_WebKit)
#define WEBKIT_EXPORTDATA __declspec(dllexport)
#else
#define WEBKIT_EXPORTDATA __declspec(dllimport)
#endif
#define WTF_EXPORT_PRIVATE
#define JS_EXPORT_PRIVATE
#define JS_EXPORTCLASS JS_EXPORTDATA
//-EAWebKitChange
#else
#define JS_EXPORTDATA
#define JS_EXPORTCLASS
#define WEBKIT_EXPORTDATA
#define WTF_EXPORT_PRIVATE
#define JS_EXPORT_PRIVATE
#endif

#endif /* USE(EXPORT_MACROS) */

#ifdef __APPLE__
#define HAVE_FUNC_USLEEP 1
#endif /* __APPLE__ */

#if OS(WINDOWS)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

// If we don't define these, they get defined in windef.h.
// We want to use std::min and std::max.
#ifndef max
#define max max
#endif
#ifndef min
#define min min
#endif

// CURL needs winsock, so don't prevent inclusion of it
#if !USE(CURL)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_ // Prevent inclusion of winsock.h in windows.h
#endif
#endif

#endif /* OS(WINDOWS) */

#ifdef __cplusplus

// These undefs match up with defines in WebCorePrefix.h for Mac OS X.
// Helps us catch if anyone uses new or delete by accident in code and doesn't include "config.h".
#undef new
#undef delete
#include <wtf/FastMalloc.h>

#endif

// On MSW, wx headers need to be included before windows.h is.
// The only way we can always ensure this is if we include wx here.
#if PLATFORM(WX)
#include <wx/defs.h>
#endif

// this breaks compilation of <QFontDatabase>, at least, so turn it off for now
// Also generates errors on wx on Windows, presumably because these functions
// are used from wx headers. On GTK+ for Mac many GTK+ files include <libintl.h>
// or <glib/gi18n-lib.h>, which in turn include <xlocale/_ctype.h> which uses
// isacii(). 
//+EAWebKitChange
//10/17/2011
// Note by Arpit Baldeva: WebKit does not want to include <ctype.h> in its code base as all functionality there is locale dependent. This is not something
// desirable to WebKit. The replacement for all the functions found in the <ctype.h> are moved to <wtf/ASCIICtype.h>.
// Update 02/17/2011 - It turns out that inclusion of this file in config.h is rather undesirable for EA port as well. config.h is included in EVERY source file in the 
// WebCore including the platform specific files. Now if the platform specific files include a platform specific existing library (say an EASTL header 
// in our case) that uses the functions defined in ctype.h, they will end up with error. This is probably the reason few platforms don't like this header file already.
#if !PLATFORM(QT) && !PLATFORM(WX) && !PLATFORM(CHROMIUM) && !(OS(DARWIN) && PLATFORM(GTK)) && !PLATFORM(EA)
#include <wtf/DisallowCType.h>
#endif
//-EAWebKitChange

#if COMPILER(MSVC)
#define SKIP_STATIC_CONSTRUCTORS_ON_MSVC 1
#elif !COMPILER(WINSCW)
#define SKIP_STATIC_CONSTRUCTORS_ON_GCC 1
#endif

#if PLATFORM(WIN)
#if PLATFORM(WIN_CAIRO)
#undef WTF_USE_CG
#define WTF_USE_CAIRO 1
#define WTF_USE_CURL 1
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_ // Prevent inclusion of winsock.h in windows.h
#endif
#elif !OS(WINCE)
#define WTF_USE_CG 1
#undef WTF_USE_CAIRO
#undef WTF_USE_CURL
#endif
#endif

#if PLATFORM(MAC)
// New theme
#define WTF_USE_NEW_THEME 1
#endif // PLATFORM(MAC)

#if OS(SYMBIAN)
#define USE_SYSTEM_MALLOC 1
#define ENABLE_PASSWORD_ECHO 1
#endif

//+EAWebKitChange
//10/19/2011 - Added PLATFORM(EA)
#if OS(UNIX) || OS(WINDOWS) || PLATFORM(EA)
#define WTF_USE_OS_RANDOMNESS 1
#endif
//-EAWebKitChange

#if PLATFORM(CHROMIUM)

// Chromium uses this file instead of JavaScriptCore/config.h to compile
// JavaScriptCore/wtf (chromium doesn't compile the rest of JSC). Therefore,
// this define is required.
#define WTF_CHANGES 1

#define WTF_USE_GOOGLEURL 1

#if !defined(WTF_USE_V8)
#define WTF_USE_V8 1
#endif

#endif /* PLATFORM(CHROMIUM) */

#if !defined(WTF_USE_V8)
#define WTF_USE_V8 0
#endif /* !defined(WTF_USE_V8) */

/* Using V8 implies not using JSC and vice versa */
#if !defined(WTF_USE_JSC)
#define WTF_USE_JSC !WTF_USE_V8
#endif

#if USE(CG)
#ifndef CGFLOAT_DEFINED
#ifdef __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif
#define CGFLOAT_DEFINED 1
#endif
#endif /* USE(CG) */

#if PLATFORM(WIN) && USE(CG)
#define WTF_USE_SAFARI_THEME 1
#endif

// CoreAnimation is available to IOS, Mac and Windows if using CG
#if PLATFORM(MAC) || PLATFORM(IOS) || (PLATFORM(WIN) && USE(CG))
#define WTF_USE_CA 1
#endif

#if PLATFORM(QT) && USE(V8) && defined(Q_WS_X11)
/* protect ourselves from evil X11 defines */
#include <bridge/npruntime_internal.h>
#endif

// FIXME: Move this to JavaScriptCore/wtf/Platform.h, which is where we define WTF_USE_AVFOUNDATION on the Mac.
// https://bugs.webkit.org/show_bug.cgi?id=67334
#if PLATFORM(WIN) && HAVE(AVCF)
#define WTF_USE_AVFOUNDATION 1
#endif
