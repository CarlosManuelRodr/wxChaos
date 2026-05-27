#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "ScriptData.h"
#include "types/EngineStatus.h"

/**
* @struct AngelscriptConfigurationEngine
* @brief Script configurator. Used to call the Configure() function.
*/
class AngelscriptConfigurationEngine
{
    ScriptData configuration;
    asIScriptEngine* engine;
    EngineStatus status;
    wxString errorInfo;
    std::string filePath;
public:
    AngelscriptConfigurationEngine();
    ~AngelscriptConfigurationEngine();

    bool CompileFromPath(const std::string& path);
    bool Execute();
    ScriptData GetScriptData();
    EngineStatus GetStatus() const;
    wxString GetErrorInfo();
};
