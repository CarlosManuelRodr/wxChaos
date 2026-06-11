#pragma once
#include "../Renderer.h"
#include "../scripting/AngelscriptRenderEngine.h"

/*
* @class RenderScriptFractal
* @brief Threaded ScriptFractal rendering routines.
*/
class ScriptFractalRenderer : public Renderer
{
    AngelscriptRenderEngine* _renderEngine;
    std::string _path;
    wxString _errorInfo;
    unsigned int _threadIndex;
    bool _hasEnded;
public:
    ScriptFractalRenderer();
    ~ScriptFractalRenderer() override;

    void Render() override;
    void PreTerminate() override;
    void SetPath(const std::string& scriptPath);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    [[nodiscard]] bool IsThereError() const;
    void SetParams(unsigned int threadIndex);
};
