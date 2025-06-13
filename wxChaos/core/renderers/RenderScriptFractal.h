#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class RenderScriptFractal
* @brief Threaded ScriptFractal rendering routines.
*/
class RenderScriptFractal : public RenderFractal
{
private:
    AngelscriptRenderEngine* renderEngine;
    std::string path;
    wxString errorInfo;
    int threadIndex;
    bool hasEnded;
public:
    RenderScriptFractal();
    ~RenderScriptFractal();

    void Render();
    void SetPath(std::string scriptPath);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    void PreTerminate();
    bool IsThereError();
    void SetParams(int _threadIndex);
};
