#pragma once
#include "../Renderer.h"
#include "../scripting/AngelscriptRenderEngine.h"

/*
* @class RenderScriptFractal
* @brief Threaded ScriptFractal rendering routines.
*/
class ScriptFractalRenderer : public Renderer
{
    AngelscriptRenderEngine* renderEngine;
    std::string path;
    wxString errorInfo;
    int threadIndex;
    bool hasEnded;
public:
    ScriptFractalRenderer();
    ~ScriptFractalRenderer() override;

    void Render() override;
    void PreTerminate() override;
    void SetPath(const std::string& scriptPath);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError() const;
    void SetParams(int _threadIndex);
};
