#pragma once

#include <atomic>
#include <angelscript.h>
#include <wx/string.h>
#include "AngelscriptRuntime.h"
#include "types/EngineStatus.h"

class Fractal;

/**
* @class AngelscriptRenderEngine
* @brief Compiles and runs an AngelScript Render() entry point.
*
* Script renderers register the current render variables as globals, compile
* the selected script, execute it for the active region, and preserve any engine
* status or error text for the caller.
*/
class AngelscriptRenderEngine
{
    ///< Process-wide AngelScript initialization guard shared by all engine types.
    AngelscriptRuntime runtime;
    asIScriptEngine* engine;
    asIScriptContext* ctx;
    std::atomic_bool abortRequested;
    EngineStatus status;
    wxString errorInfo;

    void AbortIfRequested(asIScriptContext* context) const;
    void ReleaseContext();
    void ReleaseEngine();
public:
    /**
     * @brief Creates an engine with the complete wxChaos scripting interface.
     * @param orbitFractal Fractal that receives DrawLine() calls, or nullptr when orbit drawing is inactive.
     */
    explicit AngelscriptRenderEngine(Fractal* orbitFractal = nullptr);
    ~AngelscriptRenderEngine();

    bool RegisterGlobalVariable(const char* declaration, void* pointer);
    bool CompileFromPath(const std::string& path);
    /**
     * @brief Executes a parameterless void script entry point.
     * @param entryPoint Function name without its return type or parentheses.
     * @return True when execution finishes normally or is explicitly aborted.
     */
    bool Execute(const char* entryPoint = "Render");
    void Abort();
    EngineStatus GetStatus() const;
    wxString GetErrorInfo();
};
