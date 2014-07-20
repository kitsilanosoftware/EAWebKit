/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.
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

#ifndef EAWEBKIT_CLASS_H
#define EAWEBKIT_CLASS_H

#include "Bridge.h"
#include "BridgeJSC.h"
#include <EAWebkit/EAWebkitJavascriptValue.h>

namespace JSC { namespace Bindings {

class EAClass : public Class
{
protected:
    EAClass(void) {}

public:
    static EAClass* classForObject(EA::WebKit::IJSBoundObject *obj);
    static void Finalize(void);

    virtual ~EAClass(void);

    virtual MethodList methodsNamed(const Identifier&, Instance*) const;
    virtual Field* fieldNamed(const Identifier&, Instance*) const;

private:
    EAClass(const EAClass&);            // Prohibit copying.
    EAClass& operator=(const EAClass&); // Prohibit assignment.

    mutable MethodMap mMethods;
    mutable FieldMap mFields;
};

}}

#endif
