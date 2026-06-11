#include "ScriptFractalRenderer.h"
using namespace std;

ScriptFractalRenderer::ScriptFractalRenderer()
{
    _renderEngine = nullptr;
    _threadIndex = 0;
    _hasEnded = true;
}
ScriptFractalRenderer::~ScriptFractalRenderer() = default;

void ScriptFractalRenderer::SetPath(const string& scriptPath)
{
    _path = scriptPath;
}
void ScriptFractalRenderer::Render()
{
    _hasEnded = false;

    // Creates script engine.
    _renderEngine = new AngelscriptRenderEngine();
    if (_renderEngine->GetStatus() == EngineStatus::Error)
    {
        _errorInfo = _renderEngine->GetErrorInfo();
        return;
    }

    // Register global variables
    bool isEngineOk = true;
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double minX", &_minX);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double maxX", &_maxX);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double minY", &_minY);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double maxY", &_maxY);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double xFactor", &_xFactor);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double yFactor", &_yFactor);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double kReal", &_kReal);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double kImaginary", &_kImaginary);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("int ho", &_heightOrigin);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("int hf", &_heightFinal);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("int wo", &_widthOrigin);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("int wf", &_widthFinal);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("double maxIter", &_maxIter);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("uint threadIndex", &_threadIndex);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("uint screenWidth", &_myOpt.screenWidth);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("uint screenHeight", &_myOpt.screenHeight);
    isEngineOk &= _renderEngine->RegisterGlobalVariable("uint paletteSize", &_myOpt.paletteSize);

    if (!isEngineOk)
    {
        _errorInfo = _renderEngine->GetErrorInfo();
        return;
    }

    // Compile and execute the script code.
    if (!_renderEngine->CompileFromPath(_path))
    {
        _errorInfo = _renderEngine->GetErrorInfo();
        return;
    }

    if (!_renderEngine->Execute())
    {
        _errorInfo = _renderEngine->GetErrorInfo();
        return;
    }

    _hasEnded = true;

    delete _renderEngine;
    _renderEngine = nullptr;
}
wxString ScriptFractalRenderer::GetErrorInfo()
{
    return _errorInfo;
}
void ScriptFractalRenderer::ClearErrorInfo()
{
    _errorInfo.clear();
}
bool ScriptFractalRenderer::IsThereError() const
{
    return _errorInfo.size() != 0;
}

void ScriptFractalRenderer::PreTerminate()
{
    if (!_hasEnded)
        _renderEngine->Abort();
}
void ScriptFractalRenderer::SetParams(const unsigned int threadIndex)
{
    _threadIndex = threadIndex;
}

