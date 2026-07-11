#pragma once

#include <angelscript.h>
#include <wx/string.h>
#include "AngelscriptRuntime.h"
#include "ScriptData.h"
#include "types/EngineStatus.h"

/**
* @class AngelscriptConfigurationEngine
* @brief Compiles and runs a script's Configure() entry point.
*
* The configuration engine extracts ScriptData metadata from AngelScript files
* before they are exposed as script fractals in the UI.
*/
class AngelscriptConfigurationEngine
{
    ///< Process-wide AngelScript initialization guard shared by all engine types.
    AngelscriptRuntime runtime;
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
