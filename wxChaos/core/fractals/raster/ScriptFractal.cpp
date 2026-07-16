#include "ScriptFractal.h"
#include "../../FractalFactory.h"
#include "TextUtils.h"
#include "docs/FractalDocumentation.h"
#include "../../../scripting/AngelscriptConfigurationEngine.h"
#include "../../../scripting/AngelscriptBindings.h"
using namespace std;

ScriptFractal::ScriptFractal(const unsigned int width, const unsigned int height, const ScriptData& scriptData,
                             const int renderThreads) : RasterFractal(width, height)
{
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
    ConfigurePanelOptions();
    _type = FractalType::ScriptFractal;
    _hasOrbit = scriptData.hasOrbit;
    _myRender = new ScriptFractalRenderer[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
    {
        _myRender[i].SetParams(i);
        _myRender[i].SetPath(scriptData.file);
    }
}
ScriptFractal::ScriptFractal(const unsigned int width, const unsigned int height, const string& scriptPath)
                             : RasterFractal(width, height)
{
    _path = scriptPath;
    AngelscriptConfigurationEngine configEngine;
    if (configEngine.CompileFromPath(_path) && configEngine.Execute())
    {
        const ScriptData params = configEngine.GetScriptData();
        _myScriptData = params;
        ConfigurePanelOptions();
        _minX = params.minX;
        _maxX = params.maxX;
        _minY = params.minY;
        _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;
        ConfigureIterationDefaults(params.defaultIter, 100);
    }

    _type = FractalType::ScriptFractal;
    _hasOrbit = _myScriptData.hasOrbit;
    _myRender = new ScriptFractalRenderer[_threadNumber];
    for (unsigned int i=0; i<_threadNumber; i++)
        _myRender[i].SetPath(scriptPath);
}
ScriptFractal::~ScriptFractal()
{
    this->RasterFractal::StopRender();
    delete[] _myRender;
}
const ScriptData& ScriptFractal::GetScriptData() const
{
    return _myScriptData;
}
Fractal::CoordinateSystem ScriptFractal::GetCoordinateSystem() const
{
    if (_myScriptData.horizontalCoordinate.empty() || _myScriptData.verticalCoordinate.empty())
        return Fractal::GetCoordinateSystem();

    return {wxString::FromUTF8(_myScriptData.horizontalCoordinate),
            wxString::FromUTF8(_myScriptData.verticalCoordinate)};
}
void ScriptFractal::Render()
{
    asSetMap = _setMap;
    asColorMap = _colorMap;
    for (unsigned int i = 0; i < _threadNumber; ++i)
        _myRender[i].SetScriptOptions(_myScriptData.options);
    this->SetRendererBounds<ScriptFractalRenderer>(_myRender);
}
void ScriptFractal::ConfigurePanelOptions()
{
    for (ScriptOption& option : _myScriptData.options.GetOptions())
    {
        const wxString label = wxString::FromUTF8(option.label);
        switch (option.type)
        {
            case ScriptOptionType::Integer:
                _panelOpt.LinkInteger(PanelOptionType::Spin, label, &option.integerValue,
                                      TextUtils::ToWxString(option.integerValue));
                break;
            case ScriptOptionType::Double:
                _panelOpt.LinkDouble(PanelOptionType::TextCtrl, label, &option.doubleValue,
                                     TextUtils::ToWxString(option.doubleValue));
                break;
            case ScriptOptionType::Boolean:
                _panelOpt.LinkBool(PanelOptionType::CheckBox, label, &option.boolValue,
                                   option.boolValue ? "true" : "false");
                break;
        }
    }
}
bool ScriptFractal::RegisterOrbitVariables(AngelscriptRenderEngine& engine)
{
    bool isEngineOk = true;
    isEngineOk &= engine.RegisterGlobalVariable("double minX", &_minX);
    isEngineOk &= engine.RegisterGlobalVariable("double maxX", &_maxX);
    isEngineOk &= engine.RegisterGlobalVariable("double minY", &_minY);
    isEngineOk &= engine.RegisterGlobalVariable("double maxY", &_maxY);
    isEngineOk &= engine.RegisterGlobalVariable("double xFactor", &_xFactor);
    isEngineOk &= engine.RegisterGlobalVariable("double yFactor", &_yFactor);
    isEngineOk &= engine.RegisterGlobalVariable("double kReal", &_kReal);
    isEngineOk &= engine.RegisterGlobalVariable("double kImaginary", &_kImaginary);
    isEngineOk &= engine.RegisterGlobalVariable("double orbitX", &_orbitX);
    isEngineOk &= engine.RegisterGlobalVariable("double orbitY", &_orbitY);
    isEngineOk &= engine.RegisterGlobalVariable("uint maxIterations", &_maxIterations);
    isEngineOk &= engine.RegisterGlobalVariable("uint screenWidth", &_screenWidth);
    isEngineOk &= engine.RegisterGlobalVariable("uint screenHeight", &_screenHeight);
    isEngineOk &= engine.RegisterGlobalVariable("uint paletteSize", &_paletteSize);
    return isEngineOk;
}
void ScriptFractal::DrawOrbit()
{
    AngelscriptRenderEngine orbitEngine(this, &_myScriptData.options);
    int regionBoundary = 0;
    unsigned int threadIndex = 0;
    bool isEngineOk = orbitEngine.GetStatus() != EngineStatus::Error && RegisterOrbitVariables(orbitEngine);
    isEngineOk &= orbitEngine.RegisterGlobalVariable("int ho", &regionBoundary);
    isEngineOk &= orbitEngine.RegisterGlobalVariable("int hf", &regionBoundary);
    isEngineOk &= orbitEngine.RegisterGlobalVariable("int wo", &regionBoundary);
    isEngineOk &= orbitEngine.RegisterGlobalVariable("int wf", &regionBoundary);
    isEngineOk &= orbitEngine.RegisterGlobalVariable("uint threadIndex", &threadIndex);
    isEngineOk = isEngineOk && orbitEngine.CompileFromPath(_path) && orbitEngine.Execute("DrawOrbit");

    if (!isEngineOk)
        _orbitErrorInfo = orbitEngine.GetErrorInfo();
    else
        _orbitErrorInfo.clear();

    _orbitDrawn = true;
}
void ScriptFractal::CreateInspectionFractal(FractalFactory& factory, unsigned int width, unsigned int height) const
{
    factory.CreateScriptFractal(width, height, _myScriptData);
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
wxString ScriptFractal::GetOrbitErrorInfo() const
{
    return _orbitErrorInfo;
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
