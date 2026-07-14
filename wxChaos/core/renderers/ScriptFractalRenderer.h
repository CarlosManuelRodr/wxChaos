#pragma once
#include <memory>
#include <mutex>
#include "../RasterRenderWorker.h"
#include "../scripting/AngelscriptRenderEngine.h"
#include "../../scripting/ScriptOptions.h"

/**
* @class ScriptFractalRenderer
* @brief Renders one pixel region by executing an AngelScript render script.
*
* The renderer exposes the active render variables to AngelscriptRenderEngine
* and lets the script fill the shared set, color, and auxiliary maps.
*/
class ScriptFractalRenderer : public RasterRenderWorker
{
    std::shared_ptr<AngelscriptRenderEngine> _renderEngine;
    std::mutex _renderEngineMutex;
    std::string _path;
    wxString _errorInfo;
    unsigned int _threadIndex;
    ///< Placeholder globals that let complete scripts containing DrawOrbit() compile for pixel rendering.
    double _orbitX;
    double _orbitY;
    ScriptOptions _scriptOptions; ///< Snapshot of user-editable options used by this render worker.
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
    /** @brief Copies the current script-defined options into this render worker. */
    void SetScriptOptions(const ScriptOptions& options);
};
