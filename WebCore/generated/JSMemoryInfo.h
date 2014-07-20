/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

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

#ifndef JSMemoryInfo_h
#define JSMemoryInfo_h

#include "JSDOMBinding.h"
#include "MemoryInfo.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSMemoryInfo : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSMemoryInfo* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<MemoryInfo> impl)
    {
        JSMemoryInfo* ptr = new (JSC::allocateCell<JSMemoryInfo>(globalObject->globalData().heap)) JSMemoryInfo(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);
    static const JSC::ClassInfo s_info;

    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }


    // Custom attributes
    JSC::JSValue jsHeapSizeLimit(JSC::ExecState*) const;
    MemoryInfo* impl() const { return m_impl.get(); }

private:
    RefPtr<MemoryInfo> m_impl;
protected:
    JSMemoryInfo(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<MemoryInfo>);
    void finishCreation(JSC::JSGlobalData&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, MemoryInfo*);
MemoryInfo* toMemoryInfo(JSC::JSValue);

class JSMemoryInfoPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    static JSMemoryInfoPrototype* create(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSMemoryInfoPrototype* ptr = new (JSC::allocateCell<JSMemoryInfoPrototype>(globalData.heap)) JSMemoryInfoPrototype(globalData, globalObject, structure);
        ptr->finishCreation(globalData);
        return ptr;
    }

    static const JSC::ClassInfo s_info;
    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

private:
    JSMemoryInfoPrototype(JSC::JSGlobalData& globalData, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(globalData, structure) { }
protected:
    static const unsigned StructureFlags = Base::StructureFlags;
};

// Attributes

JSC::JSValue jsMemoryInfoTotalJSHeapSize(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsMemoryInfoUsedJSHeapSize(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsMemoryInfoJsHeapSizeLimit(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);

} // namespace WebCore

#endif