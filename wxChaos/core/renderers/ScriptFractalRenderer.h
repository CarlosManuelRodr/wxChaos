#pragma once
#include <memory>
#include <mutex>
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
    std::shared_ptr<AngelscriptRenderEngine> _renderEngine;
    std::mutex _renderEngineMutex;
    std::string _path;
    wxString _errorInfo;
    unsigned int _threadIndex;
    double _orbitX;
    double _orbitY;
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
