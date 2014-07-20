
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

#include "config.h"
#include "ea_instance.h"

#include "Error.h"
#include "JSDOMBinding.h"
#include "JSGlobalObject.h"
#include "JSLock.h"
#include "ea_class.h"
#include "ea_runtime.h"
#include "runtime_object.h"

namespace JSC { namespace Bindings {


EAInstance::EAInstance(EA::WebKit::IJSBoundObject *object, PassRefPtr<RootObject> rootObject)
: Instance(rootObject)
{
    mObject = object;
    mClass = NULL;
}

EAInstance::~EAInstance() 
{
    mObject = NULL;
    mClass = NULL;
}

Class *EAInstance::getClass() const
{
    if (!mClass)
        mClass = EAClass::classForObject(mObject);
    return mClass; 
}

// Can the object be treated as a function?
bool EAInstance::supportsInvokeDefaultMethod() const
{
    return mObject->isFunction();
}

// Taken from c_instance.cpp. (This should definitely be updated whenever we update from the open source trunk.)
class EARuntimeMethod : public RuntimeMethod {
public:
    typedef RuntimeMethod Base;

    static EARuntimeMethod* create(ExecState* exec, JSGlobalObject* globalObject, const Identifier& name, Bindings::MethodList& list)
    {
        // FIXME: deprecatedGetDOMStructure uses the prototype off of the wrong global object
        // We need to pass in the right global object for "i".
        Structure* domStructure = WebCore::deprecatedGetDOMStructure<EARuntimeMethod>(exec);
        return new (allocateCell<EARuntimeMethod>(*exec->heap())) EARuntimeMethod(exec, globalObject, domStructure, name, list);
    }

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
    }

    static const ClassInfo s_info;

private:
    EARuntimeMethod(ExecState* exec, JSGlobalObject* globalObject, Structure* structure, const Identifier& name, Bindings::MethodList& list)
        : RuntimeMethod(globalObject, structure, list)
    {
        finishCreation(exec->globalData(), name);
    }

    void finishCreation(JSGlobalData& globalData, const Identifier& name)
    {
        Base::finishCreation(globalData, name);
        ASSERT(inherits(&s_info));
    }
};

const ClassInfo EARuntimeMethod::s_info = { "EARuntimeMethod", &RuntimeMethod::s_info, 0, 0 };

JSValue EAInstance::getMethod(ExecState* exec, const Identifier& propertyName)
{
    MethodList methodList = getClass()->methodsNamed(propertyName, this);
    return EARuntimeMethod::create(exec, exec->lexicalGlobalObject(), propertyName, methodList);
}

// Call a method of an object.
JSValue EAInstance::invokeMethod(ExecState* exec, RuntimeMethod* runtimeMethod)
{
    if (!asObject(runtimeMethod)->inherits(&EARuntimeMethod::s_info))
    {    
        return throwError(exec, createTypeError(exec, "Attempt to invoke non-plug-in method on plug-in object."));
    }

    const MethodList& methodList = *runtimeMethod->methods();

    // Overloading methods are not allowed.
    ASSERT(methodList.size() == 1);

    EAMethod* method = static_cast<EAMethod*>(methodList[0]);

    // We don't do this because it doesn't seem necessary, a team
    // can just return 'undefined' if they don't handle the method call.
    //NPIdentifier ident = method->identifier();
    //if (!_object->_class->hasMethod(_object, ident))
    //    return jsUndefined();

    // Translate all the arguments into JavascriptValues.
    unsigned count = exec->argumentCount();
    EAArgumentList cArgs(count);
    
    for (unsigned i = 0; i < count; i++)
    {
        cArgs[i].SetExec(exec);
        JStoEA(exec, exec->argument(i), &cArgs[i]);
    }

    // Actually call the client's function.
    bool retval = true;
    JSValue undefined = jsUndefined();
    EA::WebKit::JavascriptValue result(&undefined, exec);
    {
        JSLock::DropAllLocks dropAllLocks(SilenceAssertionsOnly);
        retval = mObject->invokeMethod(method->name(), cArgs.data(), count, &result);
    }

    if (!retval)
    {
        throwError(exec, createError(exec, "Error calling method on EAObject"));
    }

    return EAtoJS(exec, result);
}


// Call the object like it's a function.
JSValue EAInstance::invokeDefaultMethod(ExecState* exec)
{
    // The client object must identify as a function for this to work.
    if (!mObject->isFunction())
        return jsUndefined();

    // Translate the arguments.
    unsigned count = exec->argumentCount();
    EAArgumentList cArgs(count);
    for (unsigned i = 0; i < count; i++) 
    {
        cArgs[i].SetExec(exec);
        JStoEA(exec, exec->argument(i), &cArgs[i]);
    }

    // Call the object
    bool retval = true;
    JSValue undefined = jsUndefined();
    EA::WebKit::JavascriptValue result(&undefined, exec);
    {
        JSLock::DropAllLocks dropAllLocks(SilenceAssertionsOnly);
        retval = mObject->invoke(cArgs.data(), count, &result);
    }

    if (!retval)
    { 
        throwError(exec, createError(exec, "Error calling EAObject as a function."));
    }

    return EAtoJS(exec, result);
}

JSValue EAInstance::defaultValue(ExecState* exec, PreferredPrimitiveType hint) const
{
    if (hint == PreferNumber)
    {
        return jsNumber(0);
    }
    
    return valueOf(exec);
}

JSValue EAInstance::valueOf(ExecState* exec) const 
{
    return jsString(exec, mObject->stringValue());
}

void EAInstance::getPropertyNames(ExecState* exec, PropertyNameArray& nameArray)
{
	JSLock::DropAllLocks dropAllLocks(SilenceAssertionsOnly);
	EA::WebKit::IJSBoundObject::PropertyIterator* iter = mObject->First();
	while(iter)
	{
		nameArray.add(Identifier(exec,iter->GetKey()));
		iter = mObject->GetNext();
	}
}

}}
