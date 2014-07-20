/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef EAWEBKIT_EA_RUNTIME_H
#define EAWEBKIT_EA_RUNTIME_H

#include "Bridge.h"
#include "runtime_method.h"
#include <wtf/Assertions.h>
#include <internal/include/EAWebkitEASTLHelpers.h>

namespace JSC { namespace Bindings {

typedef EA::WebKit::FixedString8_32 EAIdentifier;

class EAField : public Field {
public:
    EAField(const char *ident) : mIdent(ident) {}

    virtual JSValue valueFromInstance(ExecState*, const Instance*) const;
    virtual void setValueToInstance(ExecState*, const Instance*, JSValue) const;

    const char8_t *name(void) { return mIdent.c_str(); }

private:
    EAIdentifier mIdent;
};

class EAMethod : public Method
{
public:
    EAMethod(const char *ident) : mIdent(ident) {}

    virtual int numParameters(void) const { 
		ASSERT(false);
		return 0; 
	}
    const char8_t *name(void) { return mIdent.c_str(); }

private:
    EAIdentifier mIdent;
};

}}

#endif
