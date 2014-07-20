// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "config.h"
#include "InspectorBackendDispatcher.h"
#include <wtf/text/WTFString.h>
#include <wtf/text/CString.h>

#if ENABLE(INSPECTOR)

#include "InspectorAgent.h"
#include "InspectorApplicationCacheAgent.h"
#include "InspectorCSSAgent.h"
#include "InspectorConsoleAgent.h"
#include "InspectorDOMAgent.h"
#include "InspectorDOMDebuggerAgent.h"
#include "InspectorDOMStorageAgent.h"
#include "InspectorDatabaseAgent.h"
#include "InspectorDebuggerAgent.h"
#include "InspectorFrontendChannel.h"
#include "InspectorPageAgent.h"
#include "InspectorProfilerAgent.h"
#include "InspectorResourceAgent.h"
#include "InspectorRuntimeAgent.h"
#include "InspectorTimelineAgent.h"
#include "InspectorValues.h"
#include "InspectorWorkerAgent.h"
#include "PlatformString.h"

namespace WebCore {

const char* InspectorBackendDispatcher::commandNames[] = {
    "Page.addScriptToEvaluateOnLoad",
    "Page.removeAllScriptsToEvaluateOnLoad",
    "Page.reload",
    "Page.open",
    "Page.getCookies",
    "Page.deleteCookie",
    "Page.getResourceTree",
    "Page.getResourceContent",
    "Page.searchInResources",
    "Runtime.evaluate",
    "Runtime.callFunctionOn",
    "Runtime.getProperties",
    "Runtime.releaseObject",
    "Runtime.releaseObjectGroup",
    "Console.enable",
    "Console.disable",
    "Console.clearConsoleMessages",
    "Console.setMonitoringXHREnabled",
    "Console.addInspectedNode",
    "Network.enable",
    "Network.disable",
    "Network.setUserAgentOverride",
    "Network.setExtraHeaders",
    "Network.getResourceContent",
    "Network.clearBrowserCache",
    "Network.clearBrowserCookies",
    "Network.setCacheDisabled",
    "Database.enable",
    "Database.disable",
    "Database.getDatabaseTableNames",
    "Database.executeSQL",
    "DOMStorage.enable",
    "DOMStorage.disable",
    "DOMStorage.getDOMStorageEntries",
    "DOMStorage.setDOMStorageItem",
    "DOMStorage.removeDOMStorageItem",
    "ApplicationCache.getApplicationCaches",
    "DOM.getDocument",
    "DOM.requestChildNodes",
    "DOM.querySelector",
    "DOM.querySelectorAll",
    "DOM.setNodeName",
    "DOM.setNodeValue",
    "DOM.removeNode",
    "DOM.setAttributeValue",
    "DOM.setAttributesText",
    "DOM.removeAttribute",
    "DOM.getEventListenersForNode",
    "DOM.copyNode",
    "DOM.getOuterHTML",
    "DOM.setOuterHTML",
    "DOM.performSearch",
    "DOM.cancelSearch",
    "DOM.requestNode",
    "DOM.setInspectModeEnabled",
    "DOM.highlightRect",
    "DOM.highlightNode",
    "DOM.hideHighlight",
    "DOM.highlightFrame",
    "DOM.pushNodeByPathToFrontend",
    "DOM.resolveNode",
    "DOM.getAttributes",
    "DOM.moveTo",
    "CSS.getStylesForNode",
    "CSS.getComputedStyleForNode",
    "CSS.getInlineStyleForNode",
    "CSS.getAllStyleSheets",
    "CSS.getStyleSheet",
    "CSS.getStyleSheetText",
    "CSS.setStyleSheetText",
    "CSS.setPropertyText",
    "CSS.toggleProperty",
    "CSS.setRuleSelector",
    "CSS.addRule",
    "CSS.getSupportedCSSProperties",
    "Timeline.start",
    "Timeline.stop",
    "Debugger.enable",
    "Debugger.disable",
    "Debugger.setBreakpointsActive",
    "Debugger.setBreakpointByUrl",
    "Debugger.setBreakpoint",
    "Debugger.removeBreakpoint",
    "Debugger.continueToLocation",
    "Debugger.stepOver",
    "Debugger.stepInto",
    "Debugger.stepOut",
    "Debugger.pause",
    "Debugger.resume",
    "Debugger.setScriptSource",
    "Debugger.getScriptSource",
    "Debugger.setPauseOnExceptions",
    "Debugger.evaluateOnCallFrame",
    "DOMDebugger.setDOMBreakpoint",
    "DOMDebugger.removeDOMBreakpoint",
    "DOMDebugger.setEventListenerBreakpoint",
    "DOMDebugger.removeEventListenerBreakpoint",
    "DOMDebugger.setXHRBreakpoint",
    "DOMDebugger.removeXHRBreakpoint",
    "Profiler.enable",
    "Profiler.disable",
    "Profiler.isEnabled",
    "Profiler.start",
    "Profiler.stop",
    "Profiler.getProfileHeaders",
    "Profiler.getProfile",
    "Profiler.removeProfile",
    "Profiler.clearProfiles",
    "Profiler.takeHeapSnapshot",
    "Profiler.collectGarbage",
    "Worker.setWorkerInspectionEnabled",
    "Worker.sendMessageToWorker",
    "Worker.connectToWorker",
    "Worker.disconnectFromWorker",
    "Worker.setAutoconnectToWorkers",
};


void InspectorBackendDispatcher::Page_addScriptToEvaluateOnLoad(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_scriptSource = getString(paramsContainerPtr, "scriptSource", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->addScriptToEvaluateOnLoad(&error, in_scriptSource);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.addScriptToEvaluateOnLoad"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_removeAllScriptsToEvaluateOnLoad(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_pageAgent->removeAllScriptsToEvaluateOnLoad(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.removeAllScriptsToEvaluateOnLoad"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_reload(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool ignoreCache_valueFound = false;
    bool in_ignoreCache = getBoolean(paramsContainerPtr, "ignoreCache", &ignoreCache_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->reload(&error, ignoreCache_valueFound ? &in_ignoreCache : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.reload"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_open(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_url = getString(paramsContainerPtr, "url", 0, protocolErrorsPtr);
    bool newWindow_valueFound = false;
    bool in_newWindow = getBoolean(paramsContainerPtr, "newWindow", &newWindow_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->open(&error, in_url, newWindow_valueFound ? &in_newWindow : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.open"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_getCookies(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    RefPtr<InspectorArray> out_cookies = InspectorArray::create();
    String out_cookiesString = "";

    ErrorString error;

if (!protocolErrors->length())
    m_pageAgent->getCookies(&error, &out_cookies, &out_cookiesString);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("cookies", out_cookies);
        result->setString("cookiesString", out_cookiesString);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.getCookies"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_deleteCookie(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_cookieName = getString(paramsContainerPtr, "cookieName", 0, protocolErrorsPtr);
    String in_domain = getString(paramsContainerPtr, "domain", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->deleteCookie(&error, in_cookieName, in_domain);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.deleteCookie"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_getResourceTree(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    RefPtr<InspectorObject> out_frameTree = InspectorObject::create();

    ErrorString error;

if (!protocolErrors->length())
    m_pageAgent->getResourceTree(&error, &out_frameTree);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("frameTree", out_frameTree);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.getResourceTree"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_getResourceContent(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    String out_content = "";
    bool out_base64Encoded = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_frameId = getString(paramsContainerPtr, "frameId", 0, protocolErrorsPtr);
    String in_url = getString(paramsContainerPtr, "url", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->getResourceContent(&error, in_frameId, in_url, &out_content, &out_base64Encoded);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("content", out_content);
        result->setBoolean("base64Encoded", out_base64Encoded);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.getResourceContent"), protocolErrors, error);
}

void InspectorBackendDispatcher::Page_searchInResources(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_pageAgent)
        protocolErrors->pushString("Page handler is not available.");

    RefPtr<InspectorArray> out_result = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_text = getString(paramsContainerPtr, "text", 0, protocolErrorsPtr);
    bool caseSensitive_valueFound = false;
    bool in_caseSensitive = getBoolean(paramsContainerPtr, "caseSensitive", &caseSensitive_valueFound, protocolErrorsPtr);
    bool isRegex_valueFound = false;
    bool in_isRegex = getBoolean(paramsContainerPtr, "isRegex", &isRegex_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_pageAgent->searchInResources(&error, in_text, caseSensitive_valueFound ? &in_caseSensitive : 0, isRegex_valueFound ? &in_isRegex : 0, &out_result);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("result", out_result);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Page.searchInResources"), protocolErrors, error);
}

void InspectorBackendDispatcher::Runtime_evaluate(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_runtimeAgent)
        protocolErrors->pushString("Runtime handler is not available.");

    RefPtr<InspectorObject> out_result = InspectorObject::create();
    bool out_wasThrown = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_expression = getString(paramsContainerPtr, "expression", 0, protocolErrorsPtr);
    bool objectGroup_valueFound = false;
    String in_objectGroup = getString(paramsContainerPtr, "objectGroup", &objectGroup_valueFound, protocolErrorsPtr);
    bool includeCommandLineAPI_valueFound = false;
    bool in_includeCommandLineAPI = getBoolean(paramsContainerPtr, "includeCommandLineAPI", &includeCommandLineAPI_valueFound, protocolErrorsPtr);
    bool doNotPauseOnExceptions_valueFound = false;
    bool in_doNotPauseOnExceptions = getBoolean(paramsContainerPtr, "doNotPauseOnExceptions", &doNotPauseOnExceptions_valueFound, protocolErrorsPtr);
    bool frameId_valueFound = false;
    String in_frameId = getString(paramsContainerPtr, "frameId", &frameId_valueFound, protocolErrorsPtr);
    bool returnByValue_valueFound = false;
    bool in_returnByValue = getBoolean(paramsContainerPtr, "returnByValue", &returnByValue_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_runtimeAgent->evaluate(&error, in_expression, objectGroup_valueFound ? &in_objectGroup : 0, includeCommandLineAPI_valueFound ? &in_includeCommandLineAPI : 0, doNotPauseOnExceptions_valueFound ? &in_doNotPauseOnExceptions : 0, frameId_valueFound ? &in_frameId : 0, returnByValue_valueFound ? &in_returnByValue : 0, &out_result, &out_wasThrown);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("result", out_result);
        if (out_wasThrown)
            result->setBoolean("wasThrown", out_wasThrown);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Runtime.evaluate"), protocolErrors, error);
}

void InspectorBackendDispatcher::Runtime_callFunctionOn(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_runtimeAgent)
        protocolErrors->pushString("Runtime handler is not available.");

    RefPtr<InspectorObject> out_result = InspectorObject::create();
    bool out_wasThrown = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_objectId = getString(paramsContainerPtr, "objectId", 0, protocolErrorsPtr);
    String in_functionDeclaration = getString(paramsContainerPtr, "functionDeclaration", 0, protocolErrorsPtr);
    bool arguments_valueFound = false;
    RefPtr<InspectorArray> in_arguments = getArray(paramsContainerPtr, "arguments", &arguments_valueFound, protocolErrorsPtr);
    bool returnByValue_valueFound = false;
    bool in_returnByValue = getBoolean(paramsContainerPtr, "returnByValue", &returnByValue_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_runtimeAgent->callFunctionOn(&error, in_objectId, in_functionDeclaration, arguments_valueFound ? &in_arguments : 0, returnByValue_valueFound ? &in_returnByValue : 0, &out_result, &out_wasThrown);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("result", out_result);
        if (out_wasThrown)
            result->setBoolean("wasThrown", out_wasThrown);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Runtime.callFunctionOn"), protocolErrors, error);
}

void InspectorBackendDispatcher::Runtime_getProperties(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_runtimeAgent)
        protocolErrors->pushString("Runtime handler is not available.");

    RefPtr<InspectorArray> out_result = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_objectId = getString(paramsContainerPtr, "objectId", 0, protocolErrorsPtr);
    bool ownProperties_valueFound = false;
    bool in_ownProperties = getBoolean(paramsContainerPtr, "ownProperties", &ownProperties_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_runtimeAgent->getProperties(&error, in_objectId, ownProperties_valueFound ? &in_ownProperties : 0, &out_result);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("result", out_result);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Runtime.getProperties"), protocolErrors, error);
}

void InspectorBackendDispatcher::Runtime_releaseObject(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_runtimeAgent)
        protocolErrors->pushString("Runtime handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_objectId = getString(paramsContainerPtr, "objectId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_runtimeAgent->releaseObject(&error, in_objectId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Runtime.releaseObject"), protocolErrors, error);
}

void InspectorBackendDispatcher::Runtime_releaseObjectGroup(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_runtimeAgent)
        protocolErrors->pushString("Runtime handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_objectGroup = getString(paramsContainerPtr, "objectGroup", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_runtimeAgent->releaseObjectGroup(&error, in_objectGroup);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Runtime.releaseObjectGroup"), protocolErrors, error);
}

void InspectorBackendDispatcher::Console_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_consoleAgent)
        protocolErrors->pushString("Console handler is not available.");

    int out_expiredMessagesCount = 0;

    ErrorString error;

if (!protocolErrors->length())
    m_consoleAgent->enable(&error, &out_expiredMessagesCount);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("expiredMessagesCount", out_expiredMessagesCount);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Console.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Console_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_consoleAgent)
        protocolErrors->pushString("Console handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_consoleAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Console.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Console_clearConsoleMessages(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_consoleAgent)
        protocolErrors->pushString("Console handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_consoleAgent->clearConsoleMessages(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Console.clearConsoleMessages"), protocolErrors, error);
}

void InspectorBackendDispatcher::Console_setMonitoringXHREnabled(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_consoleAgent)
        protocolErrors->pushString("Console handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_enabled = getBoolean(paramsContainerPtr, "enabled", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_consoleAgent->setMonitoringXHREnabled(&error, in_enabled);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Console.setMonitoringXHREnabled"), protocolErrors, error);
}

void InspectorBackendDispatcher::Console_addInspectedNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_consoleAgent)
        protocolErrors->pushString("Console handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_consoleAgent->addInspectedNode(&error, in_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Console.addInspectedNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_resourceAgent->enable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_resourceAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_setUserAgentOverride(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_userAgent = getString(paramsContainerPtr, "userAgent", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_resourceAgent->setUserAgentOverride(&error, in_userAgent);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.setUserAgentOverride"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_setExtraHeaders(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_headers = getObject(paramsContainerPtr, "headers", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_resourceAgent->setExtraHeaders(&error, in_headers);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.setExtraHeaders"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_getResourceContent(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    String out_content = "";
    bool out_base64Encoded = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_requestId = getString(paramsContainerPtr, "requestId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_resourceAgent->getResourceContent(&error, in_requestId, &out_content, &out_base64Encoded);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("content", out_content);
        result->setBoolean("base64Encoded", out_base64Encoded);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.getResourceContent"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_clearBrowserCache(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_resourceAgent->clearBrowserCache(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.clearBrowserCache"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_clearBrowserCookies(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_resourceAgent->clearBrowserCookies(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.clearBrowserCookies"), protocolErrors, error);
}

void InspectorBackendDispatcher::Network_setCacheDisabled(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_resourceAgent)
        protocolErrors->pushString("Network handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_cacheDisabled = getBoolean(paramsContainerPtr, "cacheDisabled", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_resourceAgent->setCacheDisabled(&error, in_cacheDisabled);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Network.setCacheDisabled"), protocolErrors, error);
}

void InspectorBackendDispatcher::Database_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_databaseAgent)
        protocolErrors->pushString("Database handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_databaseAgent->enable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Database.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Database_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_databaseAgent)
        protocolErrors->pushString("Database handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_databaseAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Database.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Database_getDatabaseTableNames(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_databaseAgent)
        protocolErrors->pushString("Database handler is not available.");

    RefPtr<InspectorArray> out_tableNames = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_databaseId = getInt(paramsContainerPtr, "databaseId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_databaseAgent->getDatabaseTableNames(&error, in_databaseId, &out_tableNames);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("tableNames", out_tableNames);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Database.getDatabaseTableNames"), protocolErrors, error);
}

void InspectorBackendDispatcher::Database_executeSQL(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_databaseAgent)
        protocolErrors->pushString("Database handler is not available.");

    bool out_success = false;
    int out_transactionId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_databaseId = getInt(paramsContainerPtr, "databaseId", 0, protocolErrorsPtr);
    String in_query = getString(paramsContainerPtr, "query", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_databaseAgent->executeSQL(&error, in_databaseId, in_query, &out_success, &out_transactionId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setBoolean("success", out_success);
        result->setNumber("transactionId", out_transactionId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Database.executeSQL"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMStorage_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domStorageAgent)
        protocolErrors->pushString("DOMStorage handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_domStorageAgent->enable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMStorage.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMStorage_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domStorageAgent)
        protocolErrors->pushString("DOMStorage handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_domStorageAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMStorage.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMStorage_getDOMStorageEntries(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domStorageAgent)
        protocolErrors->pushString("DOMStorage handler is not available.");

    RefPtr<InspectorArray> out_entries = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_storageId = getInt(paramsContainerPtr, "storageId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domStorageAgent->getDOMStorageEntries(&error, in_storageId, &out_entries);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("entries", out_entries);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMStorage.getDOMStorageEntries"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMStorage_setDOMStorageItem(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domStorageAgent)
        protocolErrors->pushString("DOMStorage handler is not available.");

    bool out_success = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_storageId = getInt(paramsContainerPtr, "storageId", 0, protocolErrorsPtr);
    String in_key = getString(paramsContainerPtr, "key", 0, protocolErrorsPtr);
    String in_value = getString(paramsContainerPtr, "value", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domStorageAgent->setDOMStorageItem(&error, in_storageId, in_key, in_value, &out_success);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setBoolean("success", out_success);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMStorage.setDOMStorageItem"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMStorage_removeDOMStorageItem(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domStorageAgent)
        protocolErrors->pushString("DOMStorage handler is not available.");

    bool out_success = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_storageId = getInt(paramsContainerPtr, "storageId", 0, protocolErrorsPtr);
    String in_key = getString(paramsContainerPtr, "key", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domStorageAgent->removeDOMStorageItem(&error, in_storageId, in_key, &out_success);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setBoolean("success", out_success);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMStorage.removeDOMStorageItem"), protocolErrors, error);
}

void InspectorBackendDispatcher::ApplicationCache_getApplicationCaches(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_applicationCacheAgent)
        protocolErrors->pushString("ApplicationCache handler is not available.");

    RefPtr<InspectorObject> out_applicationCaches = InspectorObject::create();

    ErrorString error;

if (!protocolErrors->length())
    m_applicationCacheAgent->getApplicationCaches(&error, &out_applicationCaches);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("applicationCaches", out_applicationCaches);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "ApplicationCache.getApplicationCaches"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_getDocument(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    RefPtr<InspectorObject> out_root = InspectorObject::create();

    ErrorString error;

if (!protocolErrors->length())
    m_domAgent->getDocument(&error, &out_root);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("root", out_root);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.getDocument"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_requestChildNodes(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->requestChildNodes(&error, in_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.requestChildNodes"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_querySelector(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_nodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_selector = getString(paramsContainerPtr, "selector", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->querySelector(&error, in_nodeId, in_selector, &out_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("nodeId", out_nodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.querySelector"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_querySelectorAll(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    RefPtr<InspectorArray> out_nodeIds = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_selector = getString(paramsContainerPtr, "selector", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->querySelectorAll(&error, in_nodeId, in_selector, &out_nodeIds);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("nodeIds", out_nodeIds);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.querySelectorAll"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setNodeName(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_outNodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_name = getString(paramsContainerPtr, "name", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setNodeName(&error, in_nodeId, in_name, &out_outNodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("outNodeId", out_outNodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setNodeName"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setNodeValue(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_value = getString(paramsContainerPtr, "value", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setNodeValue(&error, in_nodeId, in_value);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setNodeValue"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_removeNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->removeNode(&error, in_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.removeNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setAttributeValue(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_name = getString(paramsContainerPtr, "name", 0, protocolErrorsPtr);
    String in_value = getString(paramsContainerPtr, "value", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setAttributeValue(&error, in_nodeId, in_name, in_value);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setAttributeValue"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setAttributesText(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_text = getString(paramsContainerPtr, "text", 0, protocolErrorsPtr);
    bool name_valueFound = false;
    String in_name = getString(paramsContainerPtr, "name", &name_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setAttributesText(&error, in_nodeId, in_text, name_valueFound ? &in_name : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setAttributesText"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_removeAttribute(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_name = getString(paramsContainerPtr, "name", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->removeAttribute(&error, in_nodeId, in_name);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.removeAttribute"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_getEventListenersForNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    RefPtr<InspectorArray> out_listeners = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->getEventListenersForNode(&error, in_nodeId, &out_listeners);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("listeners", out_listeners);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.getEventListenersForNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_copyNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->copyNode(&error, in_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.copyNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_getOuterHTML(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    String out_outerHTML = "";

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->getOuterHTML(&error, in_nodeId, &out_outerHTML);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("outerHTML", out_outerHTML);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.getOuterHTML"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setOuterHTML(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_newNodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_outerHTML = getString(paramsContainerPtr, "outerHTML", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setOuterHTML(&error, in_nodeId, in_outerHTML, &out_newNodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("newNodeId", out_newNodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setOuterHTML"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_performSearch(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_query = getString(paramsContainerPtr, "query", 0, protocolErrorsPtr);
    bool runSynchronously_valueFound = false;
    bool in_runSynchronously = getBoolean(paramsContainerPtr, "runSynchronously", &runSynchronously_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->performSearch(&error, in_query, runSynchronously_valueFound ? &in_runSynchronously : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.performSearch"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_cancelSearch(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_domAgent->cancelSearch(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.cancelSearch"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_requestNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_nodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_objectId = getString(paramsContainerPtr, "objectId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->requestNode(&error, in_objectId, &out_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("nodeId", out_nodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.requestNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_setInspectModeEnabled(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_enabled = getBoolean(paramsContainerPtr, "enabled", 0, protocolErrorsPtr);
    bool highlightConfig_valueFound = false;
    RefPtr<InspectorObject> in_highlightConfig = getObject(paramsContainerPtr, "highlightConfig", &highlightConfig_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->setInspectModeEnabled(&error, in_enabled, highlightConfig_valueFound ? &in_highlightConfig : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.setInspectModeEnabled"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_highlightRect(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_x = getInt(paramsContainerPtr, "x", 0, protocolErrorsPtr);
    int in_y = getInt(paramsContainerPtr, "y", 0, protocolErrorsPtr);
    int in_width = getInt(paramsContainerPtr, "width", 0, protocolErrorsPtr);
    int in_height = getInt(paramsContainerPtr, "height", 0, protocolErrorsPtr);
    bool color_valueFound = false;
    RefPtr<InspectorObject> in_color = getObject(paramsContainerPtr, "color", &color_valueFound, protocolErrorsPtr);
    bool outlineColor_valueFound = false;
    RefPtr<InspectorObject> in_outlineColor = getObject(paramsContainerPtr, "outlineColor", &outlineColor_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->highlightRect(&error, in_x, in_y, in_width, in_height, color_valueFound ? &in_color : 0, outlineColor_valueFound ? &in_outlineColor : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.highlightRect"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_highlightNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    RefPtr<InspectorObject> in_highlightConfig = getObject(paramsContainerPtr, "highlightConfig", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->highlightNode(&error, in_nodeId, in_highlightConfig);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.highlightNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_hideHighlight(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_domAgent->hideHighlight(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.hideHighlight"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_highlightFrame(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_frameId = getString(paramsContainerPtr, "frameId", 0, protocolErrorsPtr);
    bool contentColor_valueFound = false;
    RefPtr<InspectorObject> in_contentColor = getObject(paramsContainerPtr, "contentColor", &contentColor_valueFound, protocolErrorsPtr);
    bool contentOutlineColor_valueFound = false;
    RefPtr<InspectorObject> in_contentOutlineColor = getObject(paramsContainerPtr, "contentOutlineColor", &contentOutlineColor_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->highlightFrame(&error, in_frameId, contentColor_valueFound ? &in_contentColor : 0, contentOutlineColor_valueFound ? &in_contentOutlineColor : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.highlightFrame"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_pushNodeByPathToFrontend(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_nodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_path = getString(paramsContainerPtr, "path", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->pushNodeByPathToFrontend(&error, in_path, &out_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("nodeId", out_nodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.pushNodeByPathToFrontend"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_resolveNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    RefPtr<InspectorObject> out_object = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    bool objectGroup_valueFound = false;
    String in_objectGroup = getString(paramsContainerPtr, "objectGroup", &objectGroup_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->resolveNode(&error, in_nodeId, objectGroup_valueFound ? &in_objectGroup : 0, &out_object);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("object", out_object);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.resolveNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_getAttributes(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    RefPtr<InspectorArray> out_attributes = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->getAttributes(&error, in_nodeId, &out_attributes);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("attributes", out_attributes);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.getAttributes"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOM_moveTo(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domAgent)
        protocolErrors->pushString("DOM handler is not available.");

    int out_nodeId = 0;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    int in_targetNodeId = getInt(paramsContainerPtr, "targetNodeId", 0, protocolErrorsPtr);
    bool anchorNodeId_valueFound = false;
    int in_anchorNodeId = getInt(paramsContainerPtr, "anchorNodeId", &anchorNodeId_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domAgent->moveTo(&error, in_nodeId, in_targetNodeId, anchorNodeId_valueFound ? &in_anchorNodeId : 0, &out_nodeId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setNumber("nodeId", out_nodeId);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOM.moveTo"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getStylesForNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_styles = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    bool forcedPseudoClasses_valueFound = false;
    RefPtr<InspectorArray> in_forcedPseudoClasses = getArray(paramsContainerPtr, "forcedPseudoClasses", &forcedPseudoClasses_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->getStylesForNode(&error, in_nodeId, forcedPseudoClasses_valueFound ? &in_forcedPseudoClasses : 0, &out_styles);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("styles", out_styles);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getStylesForNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getComputedStyleForNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_style = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->getComputedStyleForNode(&error, in_nodeId, &out_style);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("style", out_style);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getComputedStyleForNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getInlineStyleForNode(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_style = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->getInlineStyleForNode(&error, in_nodeId, &out_style);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("style", out_style);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getInlineStyleForNode"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getAllStyleSheets(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorArray> out_headers = InspectorArray::create();

    ErrorString error;

if (!protocolErrors->length())
    m_cssAgent->getAllStyleSheets(&error, &out_headers);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("headers", out_headers);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getAllStyleSheets"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getStyleSheet(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_styleSheet = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_styleSheetId = getString(paramsContainerPtr, "styleSheetId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->getStyleSheet(&error, in_styleSheetId, &out_styleSheet);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("styleSheet", out_styleSheet);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getStyleSheet"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getStyleSheetText(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    String out_text = "";

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_styleSheetId = getString(paramsContainerPtr, "styleSheetId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->getStyleSheetText(&error, in_styleSheetId, &out_text);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("text", out_text);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getStyleSheetText"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_setStyleSheetText(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_styleSheetId = getString(paramsContainerPtr, "styleSheetId", 0, protocolErrorsPtr);
    String in_text = getString(paramsContainerPtr, "text", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->setStyleSheetText(&error, in_styleSheetId, in_text);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.setStyleSheetText"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_setPropertyText(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_style = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_styleId = getObject(paramsContainerPtr, "styleId", 0, protocolErrorsPtr);
    int in_propertyIndex = getInt(paramsContainerPtr, "propertyIndex", 0, protocolErrorsPtr);
    String in_text = getString(paramsContainerPtr, "text", 0, protocolErrorsPtr);
    bool in_overwrite = getBoolean(paramsContainerPtr, "overwrite", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->setPropertyText(&error, in_styleId, in_propertyIndex, in_text, in_overwrite, &out_style);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("style", out_style);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.setPropertyText"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_toggleProperty(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_style = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_styleId = getObject(paramsContainerPtr, "styleId", 0, protocolErrorsPtr);
    int in_propertyIndex = getInt(paramsContainerPtr, "propertyIndex", 0, protocolErrorsPtr);
    bool in_disable = getBoolean(paramsContainerPtr, "disable", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->toggleProperty(&error, in_styleId, in_propertyIndex, in_disable, &out_style);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("style", out_style);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.toggleProperty"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_setRuleSelector(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_rule = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_ruleId = getObject(paramsContainerPtr, "ruleId", 0, protocolErrorsPtr);
    String in_selector = getString(paramsContainerPtr, "selector", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->setRuleSelector(&error, in_ruleId, in_selector, &out_rule);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("rule", out_rule);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.setRuleSelector"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_addRule(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorObject> out_rule = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_contextNodeId = getInt(paramsContainerPtr, "contextNodeId", 0, protocolErrorsPtr);
    String in_selector = getString(paramsContainerPtr, "selector", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_cssAgent->addRule(&error, in_contextNodeId, in_selector, &out_rule);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("rule", out_rule);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.addRule"), protocolErrors, error);
}

void InspectorBackendDispatcher::CSS_getSupportedCSSProperties(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_cssAgent)
        protocolErrors->pushString("CSS handler is not available.");

    RefPtr<InspectorArray> out_cssProperties = InspectorArray::create();

    ErrorString error;

if (!protocolErrors->length())
    m_cssAgent->getSupportedCSSProperties(&error, &out_cssProperties);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("cssProperties", out_cssProperties);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "CSS.getSupportedCSSProperties"), protocolErrors, error);
}

void InspectorBackendDispatcher::Timeline_start(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_timelineAgent)
        protocolErrors->pushString("Timeline handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool maxCallStackDepth_valueFound = false;
    int in_maxCallStackDepth = getInt(paramsContainerPtr, "maxCallStackDepth", &maxCallStackDepth_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_timelineAgent->start(&error, maxCallStackDepth_valueFound ? &in_maxCallStackDepth : 0);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Timeline.start"), protocolErrors, error);
}

void InspectorBackendDispatcher::Timeline_stop(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_timelineAgent)
        protocolErrors->pushString("Timeline handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_timelineAgent->stop(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Timeline.stop"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->enable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_setBreakpointsActive(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_active = getBoolean(paramsContainerPtr, "active", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->setBreakpointsActive(&error, in_active);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.setBreakpointsActive"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_setBreakpointByUrl(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    String out_breakpointId = "";
    RefPtr<InspectorArray> out_locations = InspectorArray::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_lineNumber = getInt(paramsContainerPtr, "lineNumber", 0, protocolErrorsPtr);
    bool url_valueFound = false;
    String in_url = getString(paramsContainerPtr, "url", &url_valueFound, protocolErrorsPtr);
    bool urlRegex_valueFound = false;
    String in_urlRegex = getString(paramsContainerPtr, "urlRegex", &urlRegex_valueFound, protocolErrorsPtr);
    bool columnNumber_valueFound = false;
    int in_columnNumber = getInt(paramsContainerPtr, "columnNumber", &columnNumber_valueFound, protocolErrorsPtr);
    bool condition_valueFound = false;
    String in_condition = getString(paramsContainerPtr, "condition", &condition_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->setBreakpointByUrl(&error, in_lineNumber, url_valueFound ? &in_url : 0, urlRegex_valueFound ? &in_urlRegex : 0, columnNumber_valueFound ? &in_columnNumber : 0, condition_valueFound ? &in_condition : 0, &out_breakpointId, &out_locations);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("breakpointId", out_breakpointId);
        result->setArray("locations", out_locations);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.setBreakpointByUrl"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_setBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    String out_breakpointId = "";
    RefPtr<InspectorObject> out_actualLocation = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_location = getObject(paramsContainerPtr, "location", 0, protocolErrorsPtr);
    bool condition_valueFound = false;
    String in_condition = getString(paramsContainerPtr, "condition", &condition_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->setBreakpoint(&error, in_location, condition_valueFound ? &in_condition : 0, &out_breakpointId, &out_actualLocation);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("breakpointId", out_breakpointId);
        result->setObject("actualLocation", out_actualLocation);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.setBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_removeBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_breakpointId = getString(paramsContainerPtr, "breakpointId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->removeBreakpoint(&error, in_breakpointId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.removeBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_continueToLocation(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    RefPtr<InspectorObject> in_location = getObject(paramsContainerPtr, "location", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->continueToLocation(&error, in_location);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.continueToLocation"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_stepOver(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->stepOver(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.stepOver"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_stepInto(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->stepInto(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.stepInto"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_stepOut(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->stepOut(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.stepOut"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_pause(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->pause(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.pause"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_resume(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_debuggerAgent->resume(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.resume"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_setScriptSource(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    RefPtr<InspectorArray> out_callFrames = InspectorArray::create();
    RefPtr<InspectorObject> out_result = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_scriptId = getString(paramsContainerPtr, "scriptId", 0, protocolErrorsPtr);
    String in_scriptSource = getString(paramsContainerPtr, "scriptSource", 0, protocolErrorsPtr);
    bool preview_valueFound = false;
    bool in_preview = getBoolean(paramsContainerPtr, "preview", &preview_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->setScriptSource(&error, in_scriptId, in_scriptSource, preview_valueFound ? &in_preview : 0, &out_callFrames, &out_result);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("callFrames", out_callFrames);
        result->setObject("result", out_result);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.setScriptSource"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_getScriptSource(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    String out_scriptSource = "";

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_scriptId = getString(paramsContainerPtr, "scriptId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->getScriptSource(&error, in_scriptId, &out_scriptSource);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setString("scriptSource", out_scriptSource);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.getScriptSource"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_setPauseOnExceptions(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_state = getString(paramsContainerPtr, "state", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->setPauseOnExceptions(&error, in_state);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.setPauseOnExceptions"), protocolErrors, error);
}

void InspectorBackendDispatcher::Debugger_evaluateOnCallFrame(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_debuggerAgent)
        protocolErrors->pushString("Debugger handler is not available.");

    RefPtr<InspectorObject> out_result = InspectorObject::create();
    bool out_wasThrown = false;

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_callFrameId = getString(paramsContainerPtr, "callFrameId", 0, protocolErrorsPtr);
    String in_expression = getString(paramsContainerPtr, "expression", 0, protocolErrorsPtr);
    bool objectGroup_valueFound = false;
    String in_objectGroup = getString(paramsContainerPtr, "objectGroup", &objectGroup_valueFound, protocolErrorsPtr);
    bool includeCommandLineAPI_valueFound = false;
    bool in_includeCommandLineAPI = getBoolean(paramsContainerPtr, "includeCommandLineAPI", &includeCommandLineAPI_valueFound, protocolErrorsPtr);
    bool returnByValue_valueFound = false;
    bool in_returnByValue = getBoolean(paramsContainerPtr, "returnByValue", &returnByValue_valueFound, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_debuggerAgent->evaluateOnCallFrame(&error, in_callFrameId, in_expression, objectGroup_valueFound ? &in_objectGroup : 0, includeCommandLineAPI_valueFound ? &in_includeCommandLineAPI : 0, returnByValue_valueFound ? &in_returnByValue : 0, &out_result, &out_wasThrown);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("result", out_result);
        if (out_wasThrown)
            result->setBoolean("wasThrown", out_wasThrown);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Debugger.evaluateOnCallFrame"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_setDOMBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_type = getString(paramsContainerPtr, "type", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->setDOMBreakpoint(&error, in_nodeId, in_type);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.setDOMBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_removeDOMBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_nodeId = getInt(paramsContainerPtr, "nodeId", 0, protocolErrorsPtr);
    String in_type = getString(paramsContainerPtr, "type", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->removeDOMBreakpoint(&error, in_nodeId, in_type);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.removeDOMBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_setEventListenerBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_eventName = getString(paramsContainerPtr, "eventName", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->setEventListenerBreakpoint(&error, in_eventName);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.setEventListenerBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_removeEventListenerBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_eventName = getString(paramsContainerPtr, "eventName", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->removeEventListenerBreakpoint(&error, in_eventName);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.removeEventListenerBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_setXHRBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_url = getString(paramsContainerPtr, "url", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->setXHRBreakpoint(&error, in_url);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.setXHRBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::DOMDebugger_removeXHRBreakpoint(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_domDebuggerAgent)
        protocolErrors->pushString("DOMDebugger handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_url = getString(paramsContainerPtr, "url", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_domDebuggerAgent->removeXHRBreakpoint(&error, in_url);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "DOMDebugger.removeXHRBreakpoint"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_enable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->enable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.enable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_disable(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->disable(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.disable"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_isEnabled(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    bool out_state = false;

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->isEnabled(&error, &out_state);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setBoolean("state", out_state);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.isEnabled"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_start(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->start(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.start"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_stop(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->stop(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.stop"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_getProfileHeaders(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    RefPtr<InspectorArray> out_headers = InspectorArray::create();

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->getProfileHeaders(&error, &out_headers);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setArray("headers", out_headers);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.getProfileHeaders"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_getProfile(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    RefPtr<InspectorObject> out_profile = InspectorObject::create();

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_type = getString(paramsContainerPtr, "type", 0, protocolErrorsPtr);
    int in_uid = getInt(paramsContainerPtr, "uid", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_profilerAgent->getProfile(&error, in_type, in_uid, &out_profile);

    RefPtr<InspectorObject> result = InspectorObject::create();
    if (!protocolErrors->length() && !error.length()) {
        result->setObject("profile", out_profile);
    }
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.getProfile"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_removeProfile(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    String in_type = getString(paramsContainerPtr, "type", 0, protocolErrorsPtr);
    int in_uid = getInt(paramsContainerPtr, "uid", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_profilerAgent->removeProfile(&error, in_type, in_uid);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.removeProfile"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_clearProfiles(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->clearProfiles(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.clearProfiles"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_takeHeapSnapshot(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->takeHeapSnapshot(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.takeHeapSnapshot"), protocolErrors, error);
}

void InspectorBackendDispatcher::Profiler_collectGarbage(long callId, InspectorObject*)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_profilerAgent)
        protocolErrors->pushString("Profiler handler is not available.");

    ErrorString error;

if (!protocolErrors->length())
    m_profilerAgent->collectGarbage(&error);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Profiler.collectGarbage"), protocolErrors, error);
}

void InspectorBackendDispatcher::Worker_setWorkerInspectionEnabled(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_workerAgent)
        protocolErrors->pushString("Worker handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_value = getBoolean(paramsContainerPtr, "value", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_workerAgent->setWorkerInspectionEnabled(&error, in_value);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Worker.setWorkerInspectionEnabled"), protocolErrors, error);
}

void InspectorBackendDispatcher::Worker_sendMessageToWorker(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_workerAgent)
        protocolErrors->pushString("Worker handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_workerId = getInt(paramsContainerPtr, "workerId", 0, protocolErrorsPtr);
    RefPtr<InspectorObject> in_message = getObject(paramsContainerPtr, "message", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_workerAgent->sendMessageToWorker(&error, in_workerId, in_message);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Worker.sendMessageToWorker"), protocolErrors, error);
}

void InspectorBackendDispatcher::Worker_connectToWorker(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_workerAgent)
        protocolErrors->pushString("Worker handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_workerId = getInt(paramsContainerPtr, "workerId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_workerAgent->connectToWorker(&error, in_workerId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Worker.connectToWorker"), protocolErrors, error);
}

void InspectorBackendDispatcher::Worker_disconnectFromWorker(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_workerAgent)
        protocolErrors->pushString("Worker handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    int in_workerId = getInt(paramsContainerPtr, "workerId", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_workerAgent->disconnectFromWorker(&error, in_workerId);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Worker.disconnectFromWorker"), protocolErrors, error);
}

void InspectorBackendDispatcher::Worker_setAutoconnectToWorkers(long callId, InspectorObject* requestMessageObject)
{
    RefPtr<InspectorArray> protocolErrors = InspectorArray::create();

    if (!m_workerAgent)
        protocolErrors->pushString("Worker handler is not available.");

    ErrorString error;

    RefPtr<InspectorObject> paramsContainer = requestMessageObject->getObject("params");
    InspectorObject* paramsContainerPtr = paramsContainer.get();
    InspectorArray* protocolErrorsPtr = protocolErrors.get();
    bool in_value = getBoolean(paramsContainerPtr, "value", 0, protocolErrorsPtr);

    if (!protocolErrors->length())
        m_workerAgent->setAutoconnectToWorkers(&error, in_value);

    RefPtr<InspectorObject> result = InspectorObject::create();
    sendResponse(callId, result, String::format("Some arguments of method '%s' can't be processed", "Worker.setAutoconnectToWorkers"), protocolErrors, error);
}

void InspectorBackendDispatcher::dispatch(const String& message)
{
    RefPtr<InspectorBackendDispatcher> protect = this;
    typedef void (InspectorBackendDispatcher::*CallHandler)(long callId, InspectorObject* messageObject);
    typedef HashMap<String, CallHandler> DispatchMap;
    DEFINE_STATIC_LOCAL(DispatchMap, dispatchMap, );
    long callId = 0;

    if (dispatchMap.isEmpty()) {
        static CallHandler handlers[] = {
        &InspectorBackendDispatcher::Page_addScriptToEvaluateOnLoad,
        &InspectorBackendDispatcher::Page_removeAllScriptsToEvaluateOnLoad,
        &InspectorBackendDispatcher::Page_reload,
        &InspectorBackendDispatcher::Page_open,
        &InspectorBackendDispatcher::Page_getCookies,
        &InspectorBackendDispatcher::Page_deleteCookie,
        &InspectorBackendDispatcher::Page_getResourceTree,
        &InspectorBackendDispatcher::Page_getResourceContent,
        &InspectorBackendDispatcher::Page_searchInResources,
        &InspectorBackendDispatcher::Runtime_evaluate,
        &InspectorBackendDispatcher::Runtime_callFunctionOn,
        &InspectorBackendDispatcher::Runtime_getProperties,
        &InspectorBackendDispatcher::Runtime_releaseObject,
        &InspectorBackendDispatcher::Runtime_releaseObjectGroup,
        &InspectorBackendDispatcher::Console_enable,
        &InspectorBackendDispatcher::Console_disable,
        &InspectorBackendDispatcher::Console_clearConsoleMessages,
        &InspectorBackendDispatcher::Console_setMonitoringXHREnabled,
        &InspectorBackendDispatcher::Console_addInspectedNode,
        &InspectorBackendDispatcher::Network_enable,
        &InspectorBackendDispatcher::Network_disable,
        &InspectorBackendDispatcher::Network_setUserAgentOverride,
        &InspectorBackendDispatcher::Network_setExtraHeaders,
        &InspectorBackendDispatcher::Network_getResourceContent,
        &InspectorBackendDispatcher::Network_clearBrowserCache,
        &InspectorBackendDispatcher::Network_clearBrowserCookies,
        &InspectorBackendDispatcher::Network_setCacheDisabled,
        &InspectorBackendDispatcher::Database_enable,
        &InspectorBackendDispatcher::Database_disable,
        &InspectorBackendDispatcher::Database_getDatabaseTableNames,
        &InspectorBackendDispatcher::Database_executeSQL,
        &InspectorBackendDispatcher::DOMStorage_enable,
        &InspectorBackendDispatcher::DOMStorage_disable,
        &InspectorBackendDispatcher::DOMStorage_getDOMStorageEntries,
        &InspectorBackendDispatcher::DOMStorage_setDOMStorageItem,
        &InspectorBackendDispatcher::DOMStorage_removeDOMStorageItem,
        &InspectorBackendDispatcher::ApplicationCache_getApplicationCaches,
        &InspectorBackendDispatcher::DOM_getDocument,
        &InspectorBackendDispatcher::DOM_requestChildNodes,
        &InspectorBackendDispatcher::DOM_querySelector,
        &InspectorBackendDispatcher::DOM_querySelectorAll,
        &InspectorBackendDispatcher::DOM_setNodeName,
        &InspectorBackendDispatcher::DOM_setNodeValue,
        &InspectorBackendDispatcher::DOM_removeNode,
        &InspectorBackendDispatcher::DOM_setAttributeValue,
        &InspectorBackendDispatcher::DOM_setAttributesText,
        &InspectorBackendDispatcher::DOM_removeAttribute,
        &InspectorBackendDispatcher::DOM_getEventListenersForNode,
        &InspectorBackendDispatcher::DOM_copyNode,
        &InspectorBackendDispatcher::DOM_getOuterHTML,
        &InspectorBackendDispatcher::DOM_setOuterHTML,
        &InspectorBackendDispatcher::DOM_performSearch,
        &InspectorBackendDispatcher::DOM_cancelSearch,
        &InspectorBackendDispatcher::DOM_requestNode,
        &InspectorBackendDispatcher::DOM_setInspectModeEnabled,
        &InspectorBackendDispatcher::DOM_highlightRect,
        &InspectorBackendDispatcher::DOM_highlightNode,
        &InspectorBackendDispatcher::DOM_hideHighlight,
        &InspectorBackendDispatcher::DOM_highlightFrame,
        &InspectorBackendDispatcher::DOM_pushNodeByPathToFrontend,
        &InspectorBackendDispatcher::DOM_resolveNode,
        &InspectorBackendDispatcher::DOM_getAttributes,
        &InspectorBackendDispatcher::DOM_moveTo,
        &InspectorBackendDispatcher::CSS_getStylesForNode,
        &InspectorBackendDispatcher::CSS_getComputedStyleForNode,
        &InspectorBackendDispatcher::CSS_getInlineStyleForNode,
        &InspectorBackendDispatcher::CSS_getAllStyleSheets,
        &InspectorBackendDispatcher::CSS_getStyleSheet,
        &InspectorBackendDispatcher::CSS_getStyleSheetText,
        &InspectorBackendDispatcher::CSS_setStyleSheetText,
        &InspectorBackendDispatcher::CSS_setPropertyText,
        &InspectorBackendDispatcher::CSS_toggleProperty,
        &InspectorBackendDispatcher::CSS_setRuleSelector,
        &InspectorBackendDispatcher::CSS_addRule,
        &InspectorBackendDispatcher::CSS_getSupportedCSSProperties,
        &InspectorBackendDispatcher::Timeline_start,
        &InspectorBackendDispatcher::Timeline_stop,
        &InspectorBackendDispatcher::Debugger_enable,
        &InspectorBackendDispatcher::Debugger_disable,
        &InspectorBackendDispatcher::Debugger_setBreakpointsActive,
        &InspectorBackendDispatcher::Debugger_setBreakpointByUrl,
        &InspectorBackendDispatcher::Debugger_setBreakpoint,
        &InspectorBackendDispatcher::Debugger_removeBreakpoint,
        &InspectorBackendDispatcher::Debugger_continueToLocation,
        &InspectorBackendDispatcher::Debugger_stepOver,
        &InspectorBackendDispatcher::Debugger_stepInto,
        &InspectorBackendDispatcher::Debugger_stepOut,
        &InspectorBackendDispatcher::Debugger_pause,
        &InspectorBackendDispatcher::Debugger_resume,
        &InspectorBackendDispatcher::Debugger_setScriptSource,
        &InspectorBackendDispatcher::Debugger_getScriptSource,
        &InspectorBackendDispatcher::Debugger_setPauseOnExceptions,
        &InspectorBackendDispatcher::Debugger_evaluateOnCallFrame,
        &InspectorBackendDispatcher::DOMDebugger_setDOMBreakpoint,
        &InspectorBackendDispatcher::DOMDebugger_removeDOMBreakpoint,
        &InspectorBackendDispatcher::DOMDebugger_setEventListenerBreakpoint,
        &InspectorBackendDispatcher::DOMDebugger_removeEventListenerBreakpoint,
        &InspectorBackendDispatcher::DOMDebugger_setXHRBreakpoint,
        &InspectorBackendDispatcher::DOMDebugger_removeXHRBreakpoint,
        &InspectorBackendDispatcher::Profiler_enable,
        &InspectorBackendDispatcher::Profiler_disable,
        &InspectorBackendDispatcher::Profiler_isEnabled,
        &InspectorBackendDispatcher::Profiler_start,
        &InspectorBackendDispatcher::Profiler_stop,
        &InspectorBackendDispatcher::Profiler_getProfileHeaders,
        &InspectorBackendDispatcher::Profiler_getProfile,
        &InspectorBackendDispatcher::Profiler_removeProfile,
        &InspectorBackendDispatcher::Profiler_clearProfiles,
        &InspectorBackendDispatcher::Profiler_takeHeapSnapshot,
        &InspectorBackendDispatcher::Profiler_collectGarbage,
        &InspectorBackendDispatcher::Worker_setWorkerInspectionEnabled,
        &InspectorBackendDispatcher::Worker_sendMessageToWorker,
        &InspectorBackendDispatcher::Worker_connectToWorker,
        &InspectorBackendDispatcher::Worker_disconnectFromWorker,
        &InspectorBackendDispatcher::Worker_setAutoconnectToWorkers,
        };
        size_t length = sizeof(commandNames) / sizeof(commandNames[0]);
        for (size_t i = 0; i < length; ++i)
            dispatchMap.add(commandNames[i], handlers[i]);
    }

    RefPtr<InspectorValue> parsedMessage = InspectorValue::parseJSON(message);
    if (!parsedMessage) {
        reportProtocolError(0, ParseError, "Message must be in JSON format");
        return;
    }

    RefPtr<InspectorObject> messageObject = parsedMessage->asObject();
    if (!messageObject) {
        reportProtocolError(0, InvalidRequest, "Message must be a JSONified object");
        return;
    }

    RefPtr<InspectorValue> callIdValue = messageObject->get("id");
    if (!callIdValue) {
        reportProtocolError(0, InvalidRequest, "'id' property was not found");
        return;
    }

    if (!callIdValue->asNumber(&callId)) {
        reportProtocolError(0, InvalidRequest, "The type of 'id' property must be number");
        return;
    }

    RefPtr<InspectorValue> methodValue = messageObject->get("method");
    if (!methodValue) {
        reportProtocolError(&callId, InvalidRequest, "'method' property wasn't found");
        return;
    }

    String method;
    if (!methodValue->asString(&method)) {
        reportProtocolError(&callId, InvalidRequest, "The type of 'method' property must be string");
        return;
    }

    HashMap<String, CallHandler>::iterator it = dispatchMap.find(method);
    if (it == dispatchMap.end()) {
        reportProtocolError(&callId, MethodNotFound, "'" + method + "' wasn't found");
        return;
    }

    ((*this).*it->second)(callId, messageObject.get());
}

void InspectorBackendDispatcher::sendResponse(long callId, PassRefPtr<InspectorObject> result, const String& errorMessage, PassRefPtr<InspectorArray> protocolErrors, ErrorString invocationError)
{
    if (protocolErrors->length()) {
        reportProtocolError(&callId, InvalidParams, errorMessage, protocolErrors);
        return;
    }
    if (invocationError.length()) {
        reportProtocolError(&callId, ServerError, invocationError);
        return;
    }

    RefPtr<InspectorObject> responseMessage = InspectorObject::create();
    responseMessage->setObject("result", result);
    responseMessage->setNumber("id", callId);
    if (m_inspectorFrontendChannel)
        m_inspectorFrontendChannel->sendMessageToFrontend(responseMessage->toJSONString());
}    

void InspectorBackendDispatcher::reportProtocolError(const long* const callId, CommonErrorCode code, const String& errorMessage) const
{
    reportProtocolError(callId, code, errorMessage, 0);
}

void InspectorBackendDispatcher::reportProtocolError(const long* const callId, CommonErrorCode code, const String& errorMessage, PassRefPtr<InspectorArray> data) const
{
    DEFINE_STATIC_LOCAL(Vector<int>,s_commonErrors,);
    if (!s_commonErrors.size()) {
        s_commonErrors.insert(ParseError, -32700);
        s_commonErrors.insert(InvalidRequest, -32600);
        s_commonErrors.insert(MethodNotFound, -32601);
        s_commonErrors.insert(InvalidParams, -32602);
        s_commonErrors.insert(InternalError, -32603);
        s_commonErrors.insert(ServerError, -32000);
    }
    ASSERT(code >=0);
    ASSERT((unsigned)code < s_commonErrors.size());
    ASSERT(s_commonErrors[code]);
    RefPtr<InspectorObject> error = InspectorObject::create();
    error->setNumber("code", s_commonErrors[code]);
    error->setString("message", errorMessage);
    ASSERT(error);
    if (data)
        error->setArray("data", data);
    RefPtr<InspectorObject> message = InspectorObject::create();
    message->setObject("error", error);
    if (callId)
        message->setNumber("id", *callId);
    else
        message->setValue("id", InspectorValue::null());
    if (m_inspectorFrontendChannel)
        m_inspectorFrontendChannel->sendMessageToFrontend(message->toJSONString());
}

int InspectorBackendDispatcher::getInt(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors)
{
    ASSERT(protocolErrors);

    if (valueFound)
        *valueFound = false;

    int value = 0;

    if (!object) {
        if (!valueFound) {
            // Required parameter in missing params container.
            protocolErrors->pushString(String::format("'params' object must contain required parameter '%s' with type 'Number'.", name.utf8().data()));
        }
        return value;
    }

    InspectorObject::const_iterator end = object->end();
    InspectorObject::const_iterator valueIterator = object->find(name);

    if (valueIterator == end) {
        if (!valueFound)
            protocolErrors->pushString(String::format("Parameter '%s' with type 'Number' was not found.", name.utf8().data()));
        return value;
    }

    if (!valueIterator->second->asNumber(&value))
        protocolErrors->pushString(String::format("Parameter '%s' has wrong type. It must be 'Number'.", name.utf8().data()));
    else
        if (valueFound)
            *valueFound = true;
    return value;
}

String InspectorBackendDispatcher::getString(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors)
{
    ASSERT(protocolErrors);

    if (valueFound)
        *valueFound = false;

    String value = "";

    if (!object) {
        if (!valueFound) {
            // Required parameter in missing params container.
            protocolErrors->pushString(String::format("'params' object must contain required parameter '%s' with type 'String'.", name.utf8().data()));
        }
        return value;
    }

    InspectorObject::const_iterator end = object->end();
    InspectorObject::const_iterator valueIterator = object->find(name);

    if (valueIterator == end) {
        if (!valueFound)
            protocolErrors->pushString(String::format("Parameter '%s' with type 'String' was not found.", name.utf8().data()));
        return value;
    }

    if (!valueIterator->second->asString(&value))
        protocolErrors->pushString(String::format("Parameter '%s' has wrong type. It must be 'String'.", name.utf8().data()));
    else
        if (valueFound)
            *valueFound = true;
    return value;
}

bool InspectorBackendDispatcher::getBoolean(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors)
{
    ASSERT(protocolErrors);

    if (valueFound)
        *valueFound = false;

    bool value = false;

    if (!object) {
        if (!valueFound) {
            // Required parameter in missing params container.
            protocolErrors->pushString(String::format("'params' object must contain required parameter '%s' with type 'Boolean'.", name.utf8().data()));
        }
        return value;
    }

    InspectorObject::const_iterator end = object->end();
    InspectorObject::const_iterator valueIterator = object->find(name);

    if (valueIterator == end) {
        if (!valueFound)
            protocolErrors->pushString(String::format("Parameter '%s' with type 'Boolean' was not found.", name.utf8().data()));
        return value;
    }

    if (!valueIterator->second->asBoolean(&value))
        protocolErrors->pushString(String::format("Parameter '%s' has wrong type. It must be 'Boolean'.", name.utf8().data()));
    else
        if (valueFound)
            *valueFound = true;
    return value;
}

PassRefPtr<InspectorObject> InspectorBackendDispatcher::getObject(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors)
{
    ASSERT(protocolErrors);

    if (valueFound)
        *valueFound = false;

    RefPtr<InspectorObject> value = InspectorObject::create();

    if (!object) {
        if (!valueFound) {
            // Required parameter in missing params container.
            protocolErrors->pushString(String::format("'params' object must contain required parameter '%s' with type 'Object'.", name.utf8().data()));
        }
        return value;
    }

    InspectorObject::const_iterator end = object->end();
    InspectorObject::const_iterator valueIterator = object->find(name);

    if (valueIterator == end) {
        if (!valueFound)
            protocolErrors->pushString(String::format("Parameter '%s' with type 'Object' was not found.", name.utf8().data()));
        return value;
    }

    if (!valueIterator->second->asObject(&value))
        protocolErrors->pushString(String::format("Parameter '%s' has wrong type. It must be 'Object'.", name.utf8().data()));
    else
        if (valueFound)
            *valueFound = true;
    return value;
}

PassRefPtr<InspectorArray> InspectorBackendDispatcher::getArray(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors)
{
    ASSERT(protocolErrors);

    if (valueFound)
        *valueFound = false;

    RefPtr<InspectorArray> value = InspectorArray::create();

    if (!object) {
        if (!valueFound) {
            // Required parameter in missing params container.
            protocolErrors->pushString(String::format("'params' object must contain required parameter '%s' with type 'Array'.", name.utf8().data()));
        }
        return value;
    }

    InspectorObject::const_iterator end = object->end();
    InspectorObject::const_iterator valueIterator = object->find(name);

    if (valueIterator == end) {
        if (!valueFound)
            protocolErrors->pushString(String::format("Parameter '%s' with type 'Array' was not found.", name.utf8().data()));
        return value;
    }

    if (!valueIterator->second->asArray(&value))
        protocolErrors->pushString(String::format("Parameter '%s' has wrong type. It must be 'Array'.", name.utf8().data()));
    else
        if (valueFound)
            *valueFound = true;
    return value;
}
bool InspectorBackendDispatcher::getCommandName(const String& message, String* result)
{
    RefPtr<InspectorValue> value = InspectorValue::parseJSON(message);
    if (!value)
        return false;

    RefPtr<InspectorObject> object = value->asObject();
    if (!object)
        return false;

    if (!object->getString("method", result))
        return false;

    return true;
}


} // namespace WebCore

#endif // ENABLE(INSPECTOR)
