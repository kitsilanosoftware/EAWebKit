/*
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.
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

#ifndef EAWEBKIT_EA_INSTANCE_H
#define EAWEBKIT_EA_INSTANCE_H

#include "Bridge.h"
#include "runtime_root.h"
#include "ea_class.h"
#include "ea_utility.h"
#include <EAWebkit/EAWebkitJavascriptValue.h>
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_vector.h>

namespace JSC { namespace Bindings {

typedef eastl::fixed_vector<EA::WebKit::JavascriptValue, 8> EAArgumentList;

class EAInstance : public Instance {
public:
    static PassRefPtr<EAInstance> create(EA::WebKit::IJSBoundObject* object, PassRefPtr<RootObject> rootObject)
    {
        return adoptRef(new EAInstance(object, rootObject));
    }

    virtual ~EAInstance(void);

    virtual Class *getClass(void) const;

    virtual JSValue valueOf(ExecState*) const;
    virtual JSValue defaultValue(ExecState*, PreferredPrimitiveType) const;

    virtual JSValue getMethod(ExecState* exec, const Identifier& propertyName);
    virtual JSValue invokeMethod(ExecState*, RuntimeMethod* method);

    virtual bool supportsInvokeDefaultMethod() const;
    virtual JSValue invokeDefaultMethod(ExecState*);

	virtual void getPropertyNames(ExecState* exec, PropertyNameArray& propertyNameArray);

    // Helper function to retrieve our client's implementation.
    EA::WebKit::IJSBoundObject* getObject(void) const { return mObject; }

private:
    EAInstance &operator=(const EAInstance &other); // Not implemented.

    EAInstance(EA::WebKit::IJSBoundObject*, PassRefPtr<RootObject>);

    mutable EAClass *mClass;
    EA::WebKit::IJSBoundObject *mObject;
};

}}

#endif
