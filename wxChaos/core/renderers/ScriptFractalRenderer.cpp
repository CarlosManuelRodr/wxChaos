#include "ScriptFractalRenderer.h"
using namespace std;

ScriptFractalRenderer::ScriptFractalRenderer()
{
    _renderEngine = nullptr;
    _threadIndex = 0;
    _orbitX = 0.0;
    _orbitY = 0.0;
}
ScriptFractalRenderer::~ScriptFractalRenderer() = default;

void ScriptFractalRenderer::SetPath(const string& scriptPath)
{
    _path = scriptPath;
}
void ScriptFractalRenderer::Render()
{
    // Creates script engine.
    const auto renderEngine = std::make_shared<AngelscriptRenderEngine>(nullptr, &_scriptOptions);
    {
        std::lock_guard lock(_renderEngineMutex);
        _renderEngine = renderEngine;
    }

    if (renderEngine->GetStatus() == EngineStatus::Error)
    {
        _errorInfo = renderEngine->GetErrorInfo();
        std::lock_guard lock(_renderEngineMutex);
        _renderEngine.reset();
        return;
    }

    // Register global variables
    bool isEngineOk = true;
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minX", &_minX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxX", &_maxX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minY", &_minY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxY", &_maxY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double xFactor", &_xFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double yFactor", &_yFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kReal", &_kReal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kImaginary", &_kImaginary);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double orbitX", &_orbitX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double orbitY", &_orbitY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int ho", &_heightOrigin);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int hf", &_heightFinal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wo", &_widthOrigin);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wf", &_widthFinal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxIterations", &_maxIterations);
    isEngineOk &= renderEngine->RegisterGlobalVariable("uint threadIndex", &_threadIndex);
    isEngineOk &= renderEngine->RegisterGlobalVariable("uint screenWidth", &_myOpt.screenWidth);
    isEngineOk &= renderEngine->RegisterGlobalVariable("uint screenHeight", &_myOpt.screenHeight);
    isEngineOk &= renderEngine->RegisterGlobalVariable("uint paletteSize", &_myOpt.paletteSize);

    if (!isEngineOk)
    {
        _errorInfo = renderEngine->GetErrorInfo();
        std::lock_guard lock(_renderEngineMutex);
        _renderEngine.reset();
        return;
    }

    // Compile and execute the script code.
    if (!renderEngine->CompileFromPath(_path))
    {
        _errorInfo = renderEngine->GetErrorInfo();
        std::lock_guard lock(_renderEngineMutex);
        _renderEngine.reset();
        return;
    }

    if (!renderEngine->Execute())
    {
        _errorInfo = renderEngine->GetErrorInfo();
        std::lock_guard lock(_renderEngineMutex);
        _renderEngine.reset();
        return;
    }

    std::lock_guard lock(_renderEngineMutex);
    _renderEngine.reset();
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
    std::shared_ptr<AngelscriptRenderEngine> renderEngine;
    {
        std::lock_guard lock(_renderEngineMutex);
        renderEngine = _renderEngine;
    }

    if (renderEngine != nullptr)
        renderEngine->Abort();
}
void ScriptFractalRenderer::SetParams(const unsigned int threadIndex)
{
    _threadIndex = threadIndex;
}

void ScriptFractalRenderer::SetScriptOptions(const ScriptOptions& options)
{
    _scriptOptions = options;
}

