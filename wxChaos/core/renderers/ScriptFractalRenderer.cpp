#include "ScriptFractalRenderer.h"
using namespace std;

ScriptFractalRenderer::ScriptFractalRenderer()
{
    renderEngine = nullptr;
    threadIndex = 0;
    hasEnded = true;
}
ScriptFractalRenderer::~ScriptFractalRenderer() = default;

void ScriptFractalRenderer::SetPath(const string& scriptPath)
{
    path = scriptPath;
}
void ScriptFractalRenderer::Render()
{
    hasEnded = false;

    // Creates script engine.
    renderEngine = new AngelscriptRenderEngine();
    if (renderEngine->GetStatus() == EngineStatus::Error)
    {
        errorInfo = renderEngine->GetErrorInfo();
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
    isEngineOk &= renderEngine->RegisterGlobalVariable("int ho", &_heightOrigin);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int hf", &_heightFinal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wo", &_widthOrigin);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wf", &_widthFinal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxIter", &_maxIter);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int threadIndex", &threadIndex);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenWidth", &_myOpt.screenWidth);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenHeight", &_myOpt.screenHeight);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int paletteSize", &_myOpt.paletteSize);

    if (!isEngineOk)
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    // Compile and execute the script code.
    if (!renderEngine->CompileFromPath(path))
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    if (!renderEngine->Execute())
    {
        errorInfo = renderEngine->GetErrorInfo();
        return;
    }

    hasEnded = true;

    delete renderEngine;
    renderEngine = nullptr;
}
wxString ScriptFractalRenderer::GetErrorInfo()
{
    return errorInfo;
}
void ScriptFractalRenderer::ClearErrorInfo()
{
    errorInfo.clear();
}
bool ScriptFractalRenderer::IsThereError() const
{
    return errorInfo.size() != 0;
}

void ScriptFractalRenderer::PreTerminate()
{
    if (!hasEnded)
        renderEngine->Abort();
}
void ScriptFractalRenderer::SetParams(int _threadIndex)
{
    threadIndex = _threadIndex;
}

