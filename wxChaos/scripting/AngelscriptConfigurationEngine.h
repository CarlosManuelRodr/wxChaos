#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "ScriptData.h"
#include "types/EngineStatus.h"

class AngelscriptConfigurationEngine
{
private:
    ScriptData configuration;
    asIScriptEngine* engine;
    asIScriptContext* ctx;
    EngineStatus status;
    wxString errorInfo;
    std::string filePath;
public:
    AngelscriptConfigurationEngine();
    ~AngelscriptConfigurationEngine();

    bool CompileFromPath(std::string path);
    bool Execute();
    ScriptData GetScriptData();
    EngineStatus GetStatus();
    wxString GetErrorInfo();
};
