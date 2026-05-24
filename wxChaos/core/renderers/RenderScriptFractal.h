#pragma once
#include "../RenderFractal.h"
#include "../scripting/AngelscriptRenderEngine.h"

/*
* @class RenderScriptFractal
* @brief Threaded ScriptFractal rendering routines.
*/
class RenderScriptFractal : public RenderFractal
{
    AngelscriptRenderEngine* renderEngine;
    std::string path;
    wxString errorInfo;
    int threadIndex;
    bool hasEnded;
public:
    RenderScriptFractal();
    ~RenderScriptFractal() override;

    void Render() override;
    void PreTerminate() override;
    void SetPath(std::string scriptPath);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError() const;
    void SetParams(int _threadIndex);
};
