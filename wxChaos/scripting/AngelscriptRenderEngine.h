#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "types/EngineStatus.h"

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
    asIScriptEngine* engine;
    asIScriptContext* ctx;
    EngineStatus status;
    wxString errorInfo;
public:
    AngelscriptRenderEngine();
    ~AngelscriptRenderEngine();

    bool RegisterGlobalVariable(const char* declaration, void* pointer);
    bool CompileFromPath(std::string path);
    bool Execute();
    void Abort() const;
    EngineStatus GetStatus() const;
    wxString GetErrorInfo();
};
