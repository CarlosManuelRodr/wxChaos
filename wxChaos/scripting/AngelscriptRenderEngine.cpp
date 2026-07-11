// ReSharper disable CppExpressionWithoutSideEffects
#include "AngelscriptRenderEngine.h"
#include "AngelscriptBindings.h"
#include <scriptstdstring.h>

AngelscriptRenderEngine::AngelscriptRenderEngine(Fractal* orbitFractal)
{
    engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    ctx = nullptr;
    abortRequested = false;

    if (engine == nullptr)
    {
        errorInfo = "Failed to create Angelscript engine.";
        status = EngineStatus::Error;
        return;
    }

    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);

    status = EngineStatus::Ok;

    RegisterStdString(engine);
    RegisterScriptMathReal(engine);
    RegisterScriptMathComplex(engine);
    RegisterWxChaosInterface(engine);
    RegisterOrbitDrawingInterface(engine, orbitFractal);
}

AngelscriptRenderEngine::~AngelscriptRenderEngine()
{
    ReleaseContext();
    ReleaseEngine();
}

void AngelscriptRenderEngine::AbortIfRequested(asIScriptContext* context) const
{
    if (abortRequested.load())
        context->Abort();
}

void AngelscriptRenderEngine::ReleaseContext()
{
    if (ctx != nullptr)
    {
        ctx->Release();
        ctx = nullptr;
    }
}

void AngelscriptRenderEngine::ReleaseEngine()
{
    if (engine != nullptr)
    {
        engine->Release();
        engine = nullptr;
    }
}

bool AngelscriptRenderEngine::RegisterGlobalVariable(const char* declaration, void* pointer)
{
    if (engine == nullptr)
        return false;

    if (const int r = engine->RegisterGlobalProperty(declaration, pointer); r < 0)
    {
        ReleaseEngine();
        errorInfo = "Error while registering global variable.";
        status = EngineStatus::Error;
        return false;
    }

    return true;
}

bool AngelscriptRenderEngine::CompileFromPath(const std::string& path)
{
    if (engine == nullptr)
        return false;

    if (const int r = CompileScriptFromPath(engine, path); r < 0)
    {
        ReleaseEngine();
        errorInfo = "Compile error.";
        status = EngineStatus::Error;
        return false;
    }
    return true;
}

bool AngelscriptRenderEngine::Execute(const char* entryPoint)
{
    if (engine == nullptr)
        return false;

    ctx = engine->CreateContext();
    if (ctx == nullptr)
    {
        errorInfo = "Failed to create the context.";
        ReleaseEngine();
        status = EngineStatus::Error;
        return false;
    }

    const std::string declaration = std::string("void ") + entryPoint + "()";
    asIScriptFunction* renderFunc = engine->GetModule(nullptr)->GetFunctionByDecl(declaration.c_str());
    if (renderFunc == nullptr)
    {
        errorInfo = "The function '" + wxString::FromUTF8(entryPoint) + "' was not found.";
        ReleaseContext();
        ReleaseEngine();
        status = EngineStatus::Error;
        return false;
    }

    int r = ctx->Prepare(renderFunc);
    if (r < 0)
    {
        errorInfo = "Failed to prepare the context.";
        status = EngineStatus::Error;
        ReleaseContext();
        ReleaseEngine();
        return false;
    }

    r = ctx->SetLineCallback(asMETHOD(AngelscriptRenderEngine, AbortIfRequested), this, asCALL_THISCALL);
    if (r < 0)
    {
        errorInfo = "Failed to configure the abort callback.";
        status = EngineStatus::Error;
        ReleaseContext();
        ReleaseEngine();
        return false;
    }

    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED && r != asEXECUTION_ABORTED)
    {
        errorInfo = "Script execution failed.";
        status = EngineStatus::Error;
        ReleaseContext();
        ReleaseEngine();
        asThreadCleanup();
        return false;
    }

    ReleaseContext();
    ReleaseEngine();
    asThreadCleanup();

    return true;
}

void AngelscriptRenderEngine::Abort()
{
    abortRequested = true;
}

EngineStatus AngelscriptRenderEngine::GetStatus() const
{
    return status;
}

wxString AngelscriptRenderEngine::GetErrorInfo()
{
    if (thereIsConsoleText)
    {
        thereIsConsoleText = false;
        return errorInfo + consoleText;
    }
    return errorInfo;
}
