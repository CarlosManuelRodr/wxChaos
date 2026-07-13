#pragma once
#include "../RasterFractal.h"
#include "../renderers/ScriptFractalRenderer.h"
#include "../scripting/ScriptData.h"

/**
 * @class ScriptFractal
 * @brief Fractal whose render loop is supplied by an AngelScript file.
 *
 * The script receives the current view, Julia constant, iteration count,
 * thread bounds, screen size, and palette size as globals, then writes into
 * the shared set and color maps through the scripting bindings.
 */
class ScriptFractal : public RasterFractal
{
    std::string _path;
    ScriptData _myScriptData;
    ScriptFractalRenderer* _myRender;
    ///< Error reported while compiling or executing the script's DrawOrbit() entry point.
    wxString _orbitErrorInfo;

    /**
     * @brief Registers the globals available to a script's DrawOrbit() entry point.
     * @param engine Render engine that will compile and execute the orbit script.
     * @return True when every global was registered successfully.
     */
    bool RegisterOrbitVariables(AngelscriptRenderEngine& engine);
    /** @brief Links script-declared option storage to the standard fractal options panel. */
    void ConfigurePanelOptions();

protected:
    void CreateInspectionFractal(FractalFactory& factory, unsigned int width, unsigned int height) const override;

public:
    ScriptFractal(unsigned int width, unsigned int height, const ScriptData& scriptData, int renderThreads = -1);
    ScriptFractal(unsigned int width, unsigned int height, const std::string& scriptPath);
    ~ScriptFractal() override;
    wxString GetName() const override { return "Script Fractal"; }
    CoordinateSystem GetCoordinateSystem() const override;

    void Render() override;
    /** @brief Executes the script's DrawOrbit() entry point for the selected orbit point. */
    void DrawOrbit() override;
    void PostRender() override;
    void PreRestartRender() override;
    bool IsThereError() const;
    wxString GetErrorInfo() const;
    /** @return The last error produced while compiling or executing DrawOrbit(). */
    wxString GetOrbitErrorInfo() const;
    void ClearErrorInfo() const;
    std::string GetPath();
    const ScriptData& GetScriptData() const;
    bool HasFractalInformation() const override;
    wxString GetFractalInformationFile() const override;
};
