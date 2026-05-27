#pragma once
#include "../Fractal.h"
#include "../renderers/RenderScriptFractal.h"
#include "../scripting/ScriptData.h"

/*
* @class ScriptFractal
* @brief Handles the RenderScriptdFractal threads.
*/
class ScriptFractal : public Fractal
{
    std::string _path;
    ScriptData _myScriptData;
    RenderScriptFractal* _myRender;
public:
    ScriptFractal(const sf::RenderWindow* window, const ScriptData& scriptData);
    ScriptFractal(int width, int height, const ScriptData& scriptData, int renderThreads = -1);
    ScriptFractal(int width, int height, const std::string& scriptPath);
    ~ScriptFractal() override;

    void Render() override;
    void PostRender() override;
    void PreRestartRender() override;
    bool IsThereError() const;
    wxString GetErrorInfo() const;
    void ClearErrorInfo() const;
    std::string GetPath();
};
