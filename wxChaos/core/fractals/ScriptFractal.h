#pragma once
#include "../Fractal.h"
#include "../renderers/ScriptFractalRenderer.h"
#include "../scripting/ScriptData.h"

/*
* @class ScriptFractal
* @brief Handles the RenderScriptdFractal threads.
*/
class ScriptFractal : public Fractal
{
    std::string _path;
    ScriptData _myScriptData;
    ScriptFractalRenderer* _myRender;
public:
    ScriptFractal(unsigned int width, unsigned int height, const ScriptData& scriptData, int renderThreads = -1);
    ScriptFractal(unsigned int width, unsigned int height, const std::string& scriptPath);
    ~ScriptFractal() override;
    wxString GetName() const override { return wxT("Script Fractal"); }

    void Render() override;
    void PostRender() override;
    void PreRestartRender() override;
    bool IsThereError() const;
    wxString GetErrorInfo() const;
    void ClearErrorInfo() const;
    std::string GetPath();
};
