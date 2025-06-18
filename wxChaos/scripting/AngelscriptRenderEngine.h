#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "EngineStatus.h"

class AngelscriptRenderEngine
{
private:
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
    void Abort();
    EngineStatus GetStatus();
    wxString GetErrorInfo();
};
