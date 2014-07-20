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

#ifndef JSJavaScriptAudioNode_h
#define JSJavaScriptAudioNode_h

#if ENABLE(WEB_AUDIO)

#include "JSAudioNode.h"
#include "JavaScriptAudioNode.h"
#include <runtime/JSObject.h>

namespace WebCore {

class JSJavaScriptAudioNode : public JSAudioNode {
public:
    typedef JSAudioNode Base;
    static JSJavaScriptAudioNode* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<JavaScriptAudioNode> impl)
    {
        JSJavaScriptAudioNode* ptr = new (JSC::allocateCell<JSJavaScriptAudioNode>(globalObject->globalData().heap)) JSJavaScriptAudioNode(structure, globalObject, impl);
        ptr->finishCreation(globalObject->globalData());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValue, JSC::PutPropertySlot&);
    static const JSC::ClassInfo s_info;

    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

    static JSC::JSValue getConstructor(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual void visitChildren(JSC::SlotVisitor&);

protected:
    JSJavaScriptAudioNode(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<JavaScriptAudioNode>);
    void finishCreation(JSC::JSGlobalData&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::OverridesVisitChildren | Base::StructureFlags;
};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, JavaScriptAudioNode*);

class JSJavaScriptAudioNodePrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    static JSJavaScriptAudioNodePrototype* create(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSJavaScriptAudioNodePrototype* ptr = new (JSC::allocateCell<JSJavaScriptAudioNodePrototype>(globalData.heap)) JSJavaScriptAudioNodePrototype(globalData, globalObject, structure);
        ptr->finishCreation(globalData);
        return ptr;
    }

    static const JSC::ClassInfo s_info;
    static JSC::Structure* createStructure(JSC::JSGlobalData& globalData, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(globalData, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), &s_info);
    }

private:
    JSJavaScriptAudioNodePrototype(JSC::JSGlobalData& globalData, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(globalData, structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesVisitChildren | Base::StructureFlags;
};

// Attributes

JSC::JSValue jsJavaScriptAudioNodeOnaudioprocess(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
void setJSJavaScriptAudioNodeOnaudioprocess(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsJavaScriptAudioNodeBufferSize(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsJavaScriptAudioNodeConstructor(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);

} // namespace WebCore

#endif // ENABLE(WEB_AUDIO)

#endif