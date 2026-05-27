#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "types/EngineStatus.h"

/**
* @struct AngelscriptRenderEngine
* @brief Render engine instance. Used to call the Render() function.
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
