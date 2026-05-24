#include "ScriptFractal.h"
#include "StringFuncs.h"
#include "../../scripting/AngelscriptConfigurationEngine.h"
#include "../../scripting/AngelscriptBindings.h"
using namespace std;

ScriptFractal::ScriptFractal(sf::RenderWindow* window, const ScriptData &scriptData) : Fractal(window)
{
    // Adjust the scale.
    _minX = scriptData.minX;
    _maxX = scriptData.maxX;
    _minY = scriptData.minY;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _maxIter = scriptData.defaultIter;
    _juliaVariety = scriptData.juliaVariety;
    _redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _path = scriptData.file;
    _myScriptData = scriptData;
    _type = FractalType::ScriptFractal;
    _myRender = new RenderScriptFractal[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        _myRender[i].SetParams(i);
        _myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(_myRender, &_watchdog, _threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, ScriptData scriptData, int renderThreads) 
    : Fractal(width, height)
{
    if (renderThreads != -1)
        _threadNumber = (unsigned) renderThreads;

    // Adjust the scale.
    _minX = scriptData.minX;
    _maxX = scriptData.maxX;
    _minY = scriptData.minY;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;
    this->SetOutermostZoom();

    _maxIter = scriptData.defaultIter;
    _juliaVariety = scriptData.juliaVariety;
    _redrawAlways = scriptData.redrawAlways;
    this->SetExtColorMode(scriptData.extColor);

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _path = scriptData.file;
    _myScriptData = scriptData;
    _type = FractalType::ScriptFractal;
    _myRender = new RenderScriptFractal[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        _myRender[i].SetParams(i);
        _myRender[i].SetPath(scriptData.file);
    }
    SetWatchdog<RenderScriptFractal>(_myRender, &_watchdog, _threadNumber);
}
ScriptFractal::ScriptFractal(int width, int height, string scriptPath) : Fractal(width, height)
{
    _path = scriptPath;
    AngelscriptConfigurationEngine configEngine;
    if (configEngine.CompileFromPath(_path) && configEngine.Execute())
    {
        ScriptData params = configEngine.GetScriptData();
        _minX = params.minX;
        _maxX = params.maxX;
        _minY = params.minY;
        _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
        this->SetOutermostZoom();
    }

    _type = FractalType::ScriptFractal;
    _myRender = new RenderScriptFractal[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetPath(scriptPath);
    SetWatchdog<RenderScriptFractal>(_myRender, &_watchdog, _threadNumber);
}
ScriptFractal::~ScriptFractal()
{
    this->StopRender();
    delete[] _myRender;
}
void ScriptFractal::Render()
{
    asSetMap = _setMap;
    asColorMap = _colorMap;
    asPrepareMultithread();
    this->TRender<RenderScriptFractal>(_myRender);
    asUnprepareMultithread();
}
void ScriptFractal::PostRender()
{
    wxString errorLog = wxT("");
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        if (_myRender[i].IsThereError())
        {
            errorLog += wxT("Thread ");
            errorLog += num_to_string((int)i);
            errorLog += wxT(" says:\n");
            errorLog += _myRender[i].GetErrorInfo();
            errorLog += wxT("\n");
            _myRender[i].ClearErrorInfo();
        }
    }
    if (errorLog.size() != 0)
    {
        wxString out = wxString(wxT("Fatal error in script.\n")) + errorLog;
        wxMessageDialog errorDialog(NULL, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
void ScriptFractal::PreRestartRender()
{
    // Clears all the maps.
    for (int i = 0; i < _screenWidth; i++)
    {
        for (int j = 0; j < _screenHeight; j++)
        {
            _setMap[i][j] = false;
            _colorMap[i][j] = 0;
            _auxMap[i][j] = 0;
        }
    }
}
bool ScriptFractal::IsThereError() const
{
    return _myRender[0].IsThereError();
}
wxString ScriptFractal::GetErrorInfo() const
{
    return _myRender[0].GetErrorInfo();
}
void ScriptFractal::ClearErrorInfo() const
{
    return _myRender[0].ClearErrorInfo();
}
string ScriptFractal::GetPath()
{
    return _path;
}

