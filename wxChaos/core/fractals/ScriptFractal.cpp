#include "ScriptFractal.h"
#include "TextUtils.h"
#include "docs/FractalDocumentation.h"
#include "../../scripting/AngelscriptConfigurationEngine.h"
#include "../../scripting/AngelscriptBindings.h"
using namespace std;

ScriptFractal::ScriptFractal(const unsigned int width, const unsigned int height, const ScriptData& scriptData,
                             const int renderThreads) : Fractal(width, height)
{
    asPrepareMultithread();

    if (renderThreads != -1)
        _threadNumber = static_cast<unsigned>(renderThreads);

    // Adjust the scale.
    _minX = scriptData.minX;
    _maxX = scriptData.maxX;
    _minY = scriptData.minY;
    _maxY = _minY+(_maxX-_minX)*_screenHeight/_screenWidth;

    ConfigureIterationDefaults(scriptData.defaultIter, 100);
    _juliaVariety = scriptData.juliaVariety;
    _redrawAlways = scriptData.redrawAlways;
    this->SetExteriorColorMode(scriptData.extColor);

    _xFactor = (_maxX-_minX)/(_screenWidth-1);
    _yFactor = (_maxY-_minY)/(_screenHeight-1);

    _path = scriptData.file;
    _myScriptData = scriptData;
    _type = FractalType::ScriptFractal;
    _myRender = new ScriptFractalRenderer[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        _myRender[i].SetParams(i);
        _myRender[i].SetPath(scriptData.file);
    }
}
ScriptFractal::ScriptFractal(const unsigned int width, const unsigned int height, const string& scriptPath)
                             : Fractal(width, height)
{
    asPrepareMultithread();

    _path = scriptPath;
    AngelscriptConfigurationEngine configEngine;
    if (configEngine.CompileFromPath(_path) && configEngine.Execute())
    {
        const ScriptData params = configEngine.GetScriptData();
        _myScriptData = params;
        _minX = params.minX;
        _maxX = params.maxX;
        _minY = params.minY;
        _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
        ConfigureIterationDefaults(params.defaultIter, 100);
    }

    _type = FractalType::ScriptFractal;
    _myRender = new ScriptFractalRenderer[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetPath(scriptPath);
}
ScriptFractal::~ScriptFractal()
{
    this->StopRender();
    delete[] _myRender;
    asUnprepareMultithread();
}
const ScriptData& ScriptFractal::GetScriptData() const
{
    return _myScriptData;
}
void ScriptFractal::Render()
{
    asSetMap = _setMap;
    asColorMap = _colorMap;
    this->SetRendererBounds<ScriptFractalRenderer>(_myRender);
}
void ScriptFractal::PostRender()
{
    wxString errorLog = "";
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        if (_myRender[i].IsThereError())
        {
            errorLog += "Thread ";
            errorLog += TextUtils::ToWxString(static_cast<int>(i));
            errorLog += " says:\n";
            errorLog += _myRender[i].GetErrorInfo();
            errorLog += "\n";
            _myRender[i].ClearErrorInfo();
        }
    }
    if (errorLog.size() != 0)
    {
        const wxString out = wxString("Fatal error in script.\n") + errorLog;
        wxMessageDialog errorDialog(nullptr, out, "Error", wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
void ScriptFractal::PreRestartRender()
{
    // Clears all the maps.
    for (int i = 0; i < _renderWidth; i++)
    {
        for (int j = 0; j < _renderHeight; j++)
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
bool ScriptFractal::HasFractalInformation() const
{
    return FractalDocumentation::HasDocumentation(_myScriptData);
}
wxString ScriptFractal::GetFractalInformationFile() const
{
    return FractalDocumentation::GetDocumentFile(_myScriptData);
}
