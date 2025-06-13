#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderScriptFractal::RenderScriptFractal()
{
    renderEngine = nullptr;
    threadIndex = 0;
    hasEnded = true;
}
RenderScriptFractal::~RenderScriptFractal()
{

}
void RenderScriptFractal::SetPath(string scriptPath)
{
    path = scriptPath;
}
void RenderScriptFractal::Render()
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
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minX", &minX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxX", &maxX);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double minY", &minY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxY", &maxY);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double xFactor", &xFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double yFactor", &yFactor);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kReal", &kReal);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double kImaginary", &kImaginary);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int ho", &ho);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int hf", &hf);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wo", &wo);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int wf", &wf);
    isEngineOk &= renderEngine->RegisterGlobalVariable("double maxIter", &maxIter);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int threadIndex", &threadIndex);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenWidth", &myOpt.screenWidth);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int screenHeight", &myOpt.screenHeight);
    isEngineOk &= renderEngine->RegisterGlobalVariable("int paletteSize", &myOpt.paletteSize);

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
wxString RenderScriptFractal::GetErrorInfo()
{
    return errorInfo;
}
void RenderScriptFractal::ClearErrorInfo()
{
    errorInfo.clear();
}
bool RenderScriptFractal::IsThereError()
{
    if(errorInfo.size() == 0)
        return false;
    else
        return true;
}
void RenderScriptFractal::PreTerminate()
{
    if(!hasEnded)
        renderEngine->Abort();
}
void RenderScriptFractal::SetParams(int _threadIndex)
{
    threadIndex = _threadIndex;
}

