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

#ifndef JSOESVertexArrayObject_h
#define JSOESVertexArrayObject_h

#if ENABLE(WEBGL)

#include "JSDOMBinding.h"
#include "OESVertexArrayObject.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSOESVertexArrayObject : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSOESVertexArrayObject* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<OESVertexArrayObject> impl)
    {
        JSOESVertexArrayObject* ptr = new (JSC::allocateCell<JSOESVertexArrayObject>(globalObject->globalData().heap)) JSOESVertexArrayObject(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    static const JSC::ClassInfo s_info;

    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

    OESVertexArrayObject* impl() const { return m_impl.get(); }

private:
    RefPtr<OESVertexArrayObject> m_impl;
protected:
    JSOESVertexArrayObject(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<OESVertexArrayObject>);
    void finishCreation(JSC::JSGlobalData&);
    static const unsigned StructureFlags = Base::StructureFlags;
};

class JSOESVertexArrayObjectOwner : public JSC::WeakHandleOwner {
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld*, OESVertexArrayObject*)
{
    DEFINE_STATIC_LOCAL(JSOESVertexArrayObjectOwner, jsOESVertexArrayObjectOwner, ());
    return &jsOESVertexArrayObjectOwner;
}

inline void* wrapperContext(DOMWrapperWorld* world, OESVertexArrayObject*)
{
    return world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, OESVertexArrayObject*);
OESVertexArrayObject* toOESVertexArrayObject(JSC::JSValue);

class JSOESVertexArrayObjectPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    static JSOESVertexArrayObjectPrototype* create(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSOESVertexArrayObjectPrototype* ptr = new (JSC::allocateCell<JSOESVertexArrayObjectPrototype>(globalData.heap)) JSOESVertexArrayObjectPrototype(globalData, globalObject, structure);
        ptr->finishCreation(globalData);
        return ptr;
    }

    static const JSC::ClassInfo s_info;
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier&, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier&, JSC::PropertyDescriptor&);
    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

private:
    JSOESVertexArrayObjectPrototype(JSC::JSGlobalData& globalData, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(globalData, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsOESVertexArrayObjectPrototypeFunctionCreateVertexArrayOES(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsOESVertexArrayObjectPrototypeFunctionDeleteVertexArrayOES(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsOESVertexArrayObjectPrototypeFunctionIsVertexArrayOES(JSC::ExecState*);
JSC::EncodedJSValue JSC_HOST_CALL jsOESVertexArrayObjectPrototypeFunctionBindVertexArrayOES(JSC::ExecState*);
// Constants

JSC::JSValue jsOESVertexArrayObjectVERTEX_ARRAY_BINDING_OES(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);

} // namespace WebCore

#endif // ENABLE(WEBGL)

#endif
