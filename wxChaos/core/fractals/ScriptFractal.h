#pragma once
#include "../FractalClasses.h"
#include "../renderers/RenderScriptFractal.h"

/*
* @class ScriptFractal
* @brief Handles the RenderScriptdFractal threads.
*/
class ScriptFractal : public Fractal
{
private:
    std::string path;
    ScriptData myScriptData;
    RenderScriptFractal* myRender;
public:
    ScriptFractal(sf::RenderWindow* Window, ScriptData scriptData);
    ScriptFractal(int width, int height, ScriptData scriptData, int renderThreads = -1);
    ScriptFractal(int width, int height, std::string scriptPath);
    ~ScriptFractal();

    void Render();
    void PostRender();
    void PreRestartRender();
    bool IsThereError();
    wxString GetErrorInfo();
    void ClearErrorInfo();
    std::string GetPath();
};
