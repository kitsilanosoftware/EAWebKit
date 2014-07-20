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

#include "config.h"
#include "ea_class.h"

#include "ea_instance.h"
#include "ea_runtime.h"

#include <runtime/Identifier.h>
#include <runtime/JSLock.h>
#include <wtf/text/StringHash.h>

namespace JSC { namespace Bindings {

typedef HashMap<const EA::WebKit::IJSBoundObject*, EAClass*> ClassesByObject;
static ClassesByObject* classesByObject = 0;

EAClass *EAClass::classForObject(EA::WebKit::IJSBoundObject *obj) 
{
    if (!classesByObject)
        classesByObject = new ClassesByObject;

    EAClass* aClass = classesByObject->get(obj);
    if (!aClass) {
        aClass = new EAClass();
        classesByObject->set(obj, aClass);
    }

    return aClass;
}

void EAClass::Finalize(void) {
    if (classesByObject) {
        ClassesByObject::iterator start = classesByObject->begin();
        ClassesByObject::iterator end = classesByObject->end();
        for (ClassesByObject::iterator iObj = start; iObj != end; ++iObj) {
            delete iObj->second;
        }

        classesByObject->clear();
        delete classesByObject;
        classesByObject = NULL;
    }
}

EAClass::~EAClass()
{
    JSLock lock(SilenceAssertionsOnly);

    deleteAllValues(mMethods);
    mMethods.clear();

    deleteAllValues(mFields);
    mFields.clear();
}

MethodList EAClass::methodsNamed(const Identifier& identifier, Instance* instance) const
{
    MethodList methodList;

    // Check to see if the method has been cached first.
    Method* method = mMethods.get(identifier.impl());
    if (method) 
    {
        methodList.append(method);
        return methodList;
    }

    // Method hasn't been called before, see if the object supports it.
    // Rather than doing identifier.ascii().data(), we need to create CString separately otherwise it goes out of scope.
	CString identStr = identifier.ascii();
	const char *ident = identStr.data(); 

    const EAInstance *inst = static_cast<const EAInstance*>(instance);
    EA::WebKit::IJSBoundObject *obj = inst->getObject();
    if (obj->hasMethod(ident)) 
    {
        // The object says it has this method, cache it so that the string
        // lookup can be avoided in the future.
        EAMethod* aMethod = new EAMethod(ident);
        {
            JSLock lock(SilenceAssertionsOnly);
            mMethods.set(identifier.impl(), aMethod);
        }
        methodList.append(aMethod);
    }

    return methodList;
}

Field* EAClass::fieldNamed(const Identifier& identifier, Instance* instance) const
{
    // Check to see if the field has been cached.
    Field* aField = mFields.get(identifier.impl());
    if (aField)
        return aField;
    
    // The field has not been cached, check to see if the object supports it.
	// Rather than doing identifier.ascii().data(), we need to create CString separately otherwise it goes out of scope.
	CString identStr = identifier.ascii();
	const char *ident = identStr.data(); 

    const EAInstance *inst = static_cast<const EAInstance*>(instance);
    EA::WebKit::IJSBoundObject *obj = inst->getObject();
    if (obj->hasProperty(ident))
    {
        // The object has this field, cache it.
        aField = new EAField(ident);
        {
            JSLock lock(SilenceAssertionsOnly);
            mFields.set(identifier.impl(), aField);
        }
    }

    return aField;
}

}}
