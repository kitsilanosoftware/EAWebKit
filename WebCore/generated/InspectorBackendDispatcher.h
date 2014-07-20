// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef InspectorBackendDispatcher_h
#define InspectorBackendDispatcher_h

#include <PlatformString.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class InspectorAgent;
class InspectorApplicationCacheAgent;
class InspectorArray;
class InspectorCSSAgent;
class InspectorConsoleAgent;
class InspectorDOMAgent;
class InspectorDOMDebuggerAgent;
class InspectorDOMStorageAgent;
class InspectorDatabaseAgent;
class InspectorDebuggerAgent;
class InspectorFrontendChannel;
class InspectorObject;
class InspectorPageAgent;
class InspectorProfilerAgent;
class InspectorResourceAgent;
class InspectorRuntimeAgent;
class InspectorTimelineAgent;
class InspectorWorkerAgent;

typedef String ErrorString;

class InspectorBackendDispatcher: public RefCounted<InspectorBackendDispatcher> {
public:
    InspectorBackendDispatcher(InspectorFrontendChannel* inspectorFrontendChannel, InspectorApplicationCacheAgent* applicationCacheAgent, InspectorCSSAgent* cssAgent, InspectorConsoleAgent* consoleAgent, InspectorDOMAgent* domAgent, InspectorDOMDebuggerAgent* domDebuggerAgent, InspectorDOMStorageAgent* domStorageAgent, InspectorDatabaseAgent* databaseAgent, InspectorDebuggerAgent* debuggerAgent, InspectorResourceAgent* resourceAgent, InspectorPageAgent* pageAgent, InspectorProfilerAgent* profilerAgent, InspectorRuntimeAgent* runtimeAgent, InspectorTimelineAgent* timelineAgent, InspectorWorkerAgent* workerAgent)
        : m_inspectorFrontendChannel(inspectorFrontendChannel)
        , m_applicationCacheAgent(applicationCacheAgent)
        , m_cssAgent(cssAgent)
        , m_consoleAgent(consoleAgent)
        , m_domAgent(domAgent)
        , m_domDebuggerAgent(domDebuggerAgent)
        , m_domStorageAgent(domStorageAgent)
        , m_databaseAgent(databaseAgent)
        , m_debuggerAgent(debuggerAgent)
        , m_resourceAgent(resourceAgent)
        , m_pageAgent(pageAgent)
        , m_profilerAgent(profilerAgent)
        , m_runtimeAgent(runtimeAgent)
        , m_timelineAgent(timelineAgent)
        , m_workerAgent(workerAgent)
    { }

    void clearFrontend() { m_inspectorFrontendChannel = 0; }

    enum CommonErrorCode {
        ParseError = 0,
        InvalidRequest,
        MethodNotFound,
        InvalidParams,
        InternalError,
        ServerError,
        LastEntry,
    };

    void reportProtocolError(const long* const callId, CommonErrorCode, const String& errorMessage) const;
    void reportProtocolError(const long* const callId, CommonErrorCode, const String& errorMessage, PassRefPtr<InspectorArray> data) const;
    void dispatch(const String& message);
    static bool getCommandName(const String& message, String* result);

    enum MethodNames {

        kPage_addScriptToEvaluateOnLoadCmd,
        kPage_removeAllScriptsToEvaluateOnLoadCmd,
        kPage_reloadCmd,
        kPage_openCmd,
        kPage_getCookiesCmd,
        kPage_deleteCookieCmd,
        kPage_getResourceTreeCmd,
        kPage_getResourceContentCmd,
        kPage_searchInResourcesCmd,
        kRuntime_evaluateCmd,
        kRuntime_callFunctionOnCmd,
        kRuntime_getPropertiesCmd,
        kRuntime_releaseObjectCmd,
        kRuntime_releaseObjectGroupCmd,
        kConsole_enableCmd,
        kConsole_disableCmd,
        kConsole_clearConsoleMessagesCmd,
        kConsole_setMonitoringXHREnabledCmd,
        kConsole_addInspectedNodeCmd,
        kNetwork_enableCmd,
        kNetwork_disableCmd,
        kNetwork_setUserAgentOverrideCmd,
        kNetwork_setExtraHeadersCmd,
        kNetwork_getResourceContentCmd,
        kNetwork_clearBrowserCacheCmd,
        kNetwork_clearBrowserCookiesCmd,
        kNetwork_setCacheDisabledCmd,
        kDatabase_enableCmd,
        kDatabase_disableCmd,
        kDatabase_getDatabaseTableNamesCmd,
        kDatabase_executeSQLCmd,
        kDOMStorage_enableCmd,
        kDOMStorage_disableCmd,
        kDOMStorage_getDOMStorageEntriesCmd,
        kDOMStorage_setDOMStorageItemCmd,
        kDOMStorage_removeDOMStorageItemCmd,
        kApplicationCache_getApplicationCachesCmd,
        kDOM_getDocumentCmd,
        kDOM_requestChildNodesCmd,
        kDOM_querySelectorCmd,
        kDOM_querySelectorAllCmd,
        kDOM_setNodeNameCmd,
        kDOM_setNodeValueCmd,
        kDOM_removeNodeCmd,
        kDOM_setAttributeValueCmd,
        kDOM_setAttributesTextCmd,
        kDOM_removeAttributeCmd,
        kDOM_getEventListenersForNodeCmd,
        kDOM_copyNodeCmd,
        kDOM_getOuterHTMLCmd,
        kDOM_setOuterHTMLCmd,
        kDOM_performSearchCmd,
        kDOM_cancelSearchCmd,
        kDOM_requestNodeCmd,
        kDOM_setInspectModeEnabledCmd,
        kDOM_highlightRectCmd,
        kDOM_highlightNodeCmd,
        kDOM_hideHighlightCmd,
        kDOM_highlightFrameCmd,
        kDOM_pushNodeByPathToFrontendCmd,
        kDOM_resolveNodeCmd,
        kDOM_getAttributesCmd,
        kDOM_moveToCmd,
        kCSS_getStylesForNodeCmd,
        kCSS_getComputedStyleForNodeCmd,
        kCSS_getInlineStyleForNodeCmd,
        kCSS_getAllStyleSheetsCmd,
        kCSS_getStyleSheetCmd,
        kCSS_getStyleSheetTextCmd,
        kCSS_setStyleSheetTextCmd,
        kCSS_setPropertyTextCmd,
        kCSS_togglePropertyCmd,
        kCSS_setRuleSelectorCmd,
        kCSS_addRuleCmd,
        kCSS_getSupportedCSSPropertiesCmd,
        kTimeline_startCmd,
        kTimeline_stopCmd,
        kDebugger_enableCmd,
        kDebugger_disableCmd,
        kDebugger_setBreakpointsActiveCmd,
        kDebugger_setBreakpointByUrlCmd,
        kDebugger_setBreakpointCmd,
        kDebugger_removeBreakpointCmd,
        kDebugger_continueToLocationCmd,
        kDebugger_stepOverCmd,
        kDebugger_stepIntoCmd,
        kDebugger_stepOutCmd,
        kDebugger_pauseCmd,
        kDebugger_resumeCmd,
        kDebugger_setScriptSourceCmd,
        kDebugger_getScriptSourceCmd,
        kDebugger_setPauseOnExceptionsCmd,
        kDebugger_evaluateOnCallFrameCmd,
        kDOMDebugger_setDOMBreakpointCmd,
        kDOMDebugger_removeDOMBreakpointCmd,
        kDOMDebugger_setEventListenerBreakpointCmd,
        kDOMDebugger_removeEventListenerBreakpointCmd,
        kDOMDebugger_setXHRBreakpointCmd,
        kDOMDebugger_removeXHRBreakpointCmd,
        kProfiler_enableCmd,
        kProfiler_disableCmd,
        kProfiler_isEnabledCmd,
        kProfiler_startCmd,
        kProfiler_stopCmd,
        kProfiler_getProfileHeadersCmd,
        kProfiler_getProfileCmd,
        kProfiler_removeProfileCmd,
        kProfiler_clearProfilesCmd,
        kProfiler_takeHeapSnapshotCmd,
        kProfiler_collectGarbageCmd,
        kWorker_setWorkerInspectionEnabledCmd,
        kWorker_sendMessageToWorkerCmd,
        kWorker_connectToWorkerCmd,
        kWorker_disconnectFromWorkerCmd,
        kWorker_setAutoconnectToWorkersCmd,
};

    static const char* commandNames[];

private:
    static int getInt(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors);
    static String getString(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors);
    static bool getBoolean(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors);
    static PassRefPtr<InspectorObject> getObject(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors);
    static PassRefPtr<InspectorArray> getArray(InspectorObject* object, const String& name, bool* valueFound, InspectorArray* protocolErrors);
    void sendResponse(long callId, PassRefPtr<InspectorObject> result, const String& errorMessage, PassRefPtr<InspectorArray> protocolErrors, ErrorString invocationError);

    void Page_addScriptToEvaluateOnLoad(long callId, InspectorObject* requestMessageObject);
    void Page_removeAllScriptsToEvaluateOnLoad(long callId, InspectorObject* requestMessageObject);
    void Page_reload(long callId, InspectorObject* requestMessageObject);
    void Page_open(long callId, InspectorObject* requestMessageObject);
    void Page_getCookies(long callId, InspectorObject* requestMessageObject);
    void Page_deleteCookie(long callId, InspectorObject* requestMessageObject);
    void Page_getResourceTree(long callId, InspectorObject* requestMessageObject);
    void Page_getResourceContent(long callId, InspectorObject* requestMessageObject);
    void Page_searchInResources(long callId, InspectorObject* requestMessageObject);
    void Runtime_evaluate(long callId, InspectorObject* requestMessageObject);
    void Runtime_callFunctionOn(long callId, InspectorObject* requestMessageObject);
    void Runtime_getProperties(long callId, InspectorObject* requestMessageObject);
    void Runtime_releaseObject(long callId, InspectorObject* requestMessageObject);
    void Runtime_releaseObjectGroup(long callId, InspectorObject* requestMessageObject);
    void Console_enable(long callId, InspectorObject* requestMessageObject);
    void Console_disable(long callId, InspectorObject* requestMessageObject);
    void Console_clearConsoleMessages(long callId, InspectorObject* requestMessageObject);
    void Console_setMonitoringXHREnabled(long callId, InspectorObject* requestMessageObject);
    void Console_addInspectedNode(long callId, InspectorObject* requestMessageObject);
    void Network_enable(long callId, InspectorObject* requestMessageObject);
    void Network_disable(long callId, InspectorObject* requestMessageObject);
    void Network_setUserAgentOverride(long callId, InspectorObject* requestMessageObject);
    void Network_setExtraHeaders(long callId, InspectorObject* requestMessageObject);
    void Network_getResourceContent(long callId, InspectorObject* requestMessageObject);
    void Network_clearBrowserCache(long callId, InspectorObject* requestMessageObject);
    void Network_clearBrowserCookies(long callId, InspectorObject* requestMessageObject);
    void Network_setCacheDisabled(long callId, InspectorObject* requestMessageObject);
    void Database_enable(long callId, InspectorObject* requestMessageObject);
    void Database_disable(long callId, InspectorObject* requestMessageObject);
    void Database_getDatabaseTableNames(long callId, InspectorObject* requestMessageObject);
    void Database_executeSQL(long callId, InspectorObject* requestMessageObject);
    void DOMStorage_enable(long callId, InspectorObject* requestMessageObject);
    void DOMStorage_disable(long callId, InspectorObject* requestMessageObject);
    void DOMStorage_getDOMStorageEntries(long callId, InspectorObject* requestMessageObject);
    void DOMStorage_setDOMStorageItem(long callId, InspectorObject* requestMessageObject);
    void DOMStorage_removeDOMStorageItem(long callId, InspectorObject* requestMessageObject);
    void ApplicationCache_getApplicationCaches(long callId, InspectorObject* requestMessageObject);
    void DOM_getDocument(long callId, InspectorObject* requestMessageObject);
    void DOM_requestChildNodes(long callId, InspectorObject* requestMessageObject);
    void DOM_querySelector(long callId, InspectorObject* requestMessageObject);
    void DOM_querySelectorAll(long callId, InspectorObject* requestMessageObject);
    void DOM_setNodeName(long callId, InspectorObject* requestMessageObject);
    void DOM_setNodeValue(long callId, InspectorObject* requestMessageObject);
    void DOM_removeNode(long callId, InspectorObject* requestMessageObject);
    void DOM_setAttributeValue(long callId, InspectorObject* requestMessageObject);
    void DOM_setAttributesText(long callId, InspectorObject* requestMessageObject);
    void DOM_removeAttribute(long callId, InspectorObject* requestMessageObject);
    void DOM_getEventListenersForNode(long callId, InspectorObject* requestMessageObject);
    void DOM_copyNode(long callId, InspectorObject* requestMessageObject);
    void DOM_getOuterHTML(long callId, InspectorObject* requestMessageObject);
    void DOM_setOuterHTML(long callId, InspectorObject* requestMessageObject);
    void DOM_performSearch(long callId, InspectorObject* requestMessageObject);
    void DOM_cancelSearch(long callId, InspectorObject* requestMessageObject);
    void DOM_requestNode(long callId, InspectorObject* requestMessageObject);
    void DOM_setInspectModeEnabled(long callId, InspectorObject* requestMessageObject);
    void DOM_highlightRect(long callId, InspectorObject* requestMessageObject);
    void DOM_highlightNode(long callId, InspectorObject* requestMessageObject);
    void DOM_hideHighlight(long callId, InspectorObject* requestMessageObject);
    void DOM_highlightFrame(long callId, InspectorObject* requestMessageObject);
    void DOM_pushNodeByPathToFrontend(long callId, InspectorObject* requestMessageObject);
    void DOM_resolveNode(long callId, InspectorObject* requestMessageObject);
    void DOM_getAttributes(long callId, InspectorObject* requestMessageObject);
    void DOM_moveTo(long callId, InspectorObject* requestMessageObject);
    void CSS_getStylesForNode(long callId, InspectorObject* requestMessageObject);
    void CSS_getComputedStyleForNode(long callId, InspectorObject* requestMessageObject);
    void CSS_getInlineStyleForNode(long callId, InspectorObject* requestMessageObject);
    void CSS_getAllStyleSheets(long callId, InspectorObject* requestMessageObject);
    void CSS_getStyleSheet(long callId, InspectorObject* requestMessageObject);
    void CSS_getStyleSheetText(long callId, InspectorObject* requestMessageObject);
    void CSS_setStyleSheetText(long callId, InspectorObject* requestMessageObject);
    void CSS_setPropertyText(long callId, InspectorObject* requestMessageObject);
    void CSS_toggleProperty(long callId, InspectorObject* requestMessageObject);
    void CSS_setRuleSelector(long callId, InspectorObject* requestMessageObject);
    void CSS_addRule(long callId, InspectorObject* requestMessageObject);
    void CSS_getSupportedCSSProperties(long callId, InspectorObject* requestMessageObject);
    void Timeline_start(long callId, InspectorObject* requestMessageObject);
    void Timeline_stop(long callId, InspectorObject* requestMessageObject);
    void Debugger_enable(long callId, InspectorObject* requestMessageObject);
    void Debugger_disable(long callId, InspectorObject* requestMessageObject);
    void Debugger_setBreakpointsActive(long callId, InspectorObject* requestMessageObject);
    void Debugger_setBreakpointByUrl(long callId, InspectorObject* requestMessageObject);
    void Debugger_setBreakpoint(long callId, InspectorObject* requestMessageObject);
    void Debugger_removeBreakpoint(long callId, InspectorObject* requestMessageObject);
    void Debugger_continueToLocation(long callId, InspectorObject* requestMessageObject);
    void Debugger_stepOver(long callId, InspectorObject* requestMessageObject);
    void Debugger_stepInto(long callId, InspectorObject* requestMessageObject);
    void Debugger_stepOut(long callId, InspectorObject* requestMessageObject);
    void Debugger_pause(long callId, InspectorObject* requestMessageObject);
    void Debugger_resume(long callId, InspectorObject* requestMessageObject);
    void Debugger_setScriptSource(long callId, InspectorObject* requestMessageObject);
    void Debugger_getScriptSource(long callId, InspectorObject* requestMessageObject);
    void Debugger_setPauseOnExceptions(long callId, InspectorObject* requestMessageObject);
    void Debugger_evaluateOnCallFrame(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_setDOMBreakpoint(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_removeDOMBreakpoint(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_setEventListenerBreakpoint(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_removeEventListenerBreakpoint(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_setXHRBreakpoint(long callId, InspectorObject* requestMessageObject);
    void DOMDebugger_removeXHRBreakpoint(long callId, InspectorObject* requestMessageObject);
    void Profiler_enable(long callId, InspectorObject* requestMessageObject);
    void Profiler_disable(long callId, InspectorObject* requestMessageObject);
    void Profiler_isEnabled(long callId, InspectorObject* requestMessageObject);
    void Profiler_start(long callId, InspectorObject* requestMessageObject);
    void Profiler_stop(long callId, InspectorObject* requestMessageObject);
    void Profiler_getProfileHeaders(long callId, InspectorObject* requestMessageObject);
    void Profiler_getProfile(long callId, InspectorObject* requestMessageObject);
    void Profiler_removeProfile(long callId, InspectorObject* requestMessageObject);
    void Profiler_clearProfiles(long callId, InspectorObject* requestMessageObject);
    void Profiler_takeHeapSnapshot(long callId, InspectorObject* requestMessageObject);
    void Profiler_collectGarbage(long callId, InspectorObject* requestMessageObject);
    void Worker_setWorkerInspectionEnabled(long callId, InspectorObject* requestMessageObject);
    void Worker_sendMessageToWorker(long callId, InspectorObject* requestMessageObject);
    void Worker_connectToWorker(long callId, InspectorObject* requestMessageObject);
    void Worker_disconnectFromWorker(long callId, InspectorObject* requestMessageObject);
    void Worker_setAutoconnectToWorkers(long callId, InspectorObject* requestMessageObject);

    InspectorFrontendChannel* m_inspectorFrontendChannel;
    InspectorApplicationCacheAgent* m_applicationCacheAgent;
    InspectorCSSAgent* m_cssAgent;
    InspectorConsoleAgent* m_consoleAgent;
    InspectorDOMAgent* m_domAgent;
    InspectorDOMDebuggerAgent* m_domDebuggerAgent;
    InspectorDOMStorageAgent* m_domStorageAgent;
    InspectorDatabaseAgent* m_databaseAgent;
    InspectorDebuggerAgent* m_debuggerAgent;
    InspectorResourceAgent* m_resourceAgent;
    InspectorPageAgent* m_pageAgent;
    InspectorProfilerAgent* m_profilerAgent;
    InspectorRuntimeAgent* m_runtimeAgent;
    InspectorTimelineAgent* m_timelineAgent;
    InspectorWorkerAgent* m_workerAgent;
};

} // namespace WebCore
#endif // !defined(InspectorBackendDispatcher_h)

