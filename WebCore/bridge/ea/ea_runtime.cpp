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

#include "config.h"
#include "ea_runtime.h"
#include "ea_utility.h"
#include "ea_instance.h"

namespace JSC { namespace Bindings {

    JSValue EAField::valueFromInstance(ExecState* exec, const Instance* inst) const
    {
        const EAInstance *instance = static_cast<const EAInstance *>(inst);
        EA::WebKit::IJSBoundObject *obj = instance->getObject();

        JSValue undefined = jsUndefined();
        EA::WebKit::JavascriptValue result(&undefined, exec);
        obj->getProperty(mIdent.c_str(), &result);

        return EAtoJS(exec, result);
    }

    void EAField::setValueToInstance(ExecState* exec, const Instance* inst, JSValue aValue) const
    {
        const EAInstance *instance = static_cast<const EAInstance *>(inst);
        EA::WebKit::IJSBoundObject *obj = instance->getObject();

        JSValue undefined = jsUndefined();
        EA::WebKit::JavascriptValue value(&undefined, exec);
        JStoEA(exec, aValue, &value);
        obj->setProperty(mIdent.c_str(), value);
    }

}}
