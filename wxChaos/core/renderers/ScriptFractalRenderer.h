#pragma once
#include "../RenderWorker.h"
#include "../scripting/AngelscriptRenderEngine.h"

/**
* @class ScriptFractalRenderer
* @brief Renders one pixel region by executing an AngelScript render script.
*
* The renderer exposes the active render variables to AngelscriptRenderEngine
* and lets the script fill the shared set, color, and auxiliary maps.
*/
class ScriptFractalRenderer : public RenderWorker
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
