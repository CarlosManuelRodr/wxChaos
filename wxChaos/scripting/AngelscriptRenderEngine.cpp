#include "AngelscriptRenderEngine.h"
#include "AngelscriptBindings.h"
#include <scriptstdstring.h>
#include <utility>

AngelscriptRenderEngine::AngelscriptRenderEngine()
{
    engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, false);
    engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);
    if (engine == nullptr)
    {
        errorInfo = wxT("Failed to create Angelscript engine.");
        status = EngineStatus::Error;
        return;
    }
    else
        status = EngineStatus::Ok;

    RegisterStdString(engine);
    RegisterScriptMathReal(engine);
    RegisterScriptMathComplex(engine);
    RegisterWxChaosInterface(engine);

    ctx = nullptr;
}

AngelscriptRenderEngine::~AngelscriptRenderEngine()
{
    if (engine != nullptr)
    {
        engine->Release();
        engine = nullptr;
    }
}

bool AngelscriptRenderEngine::RegisterGlobalVariable(const char* declaration, void* pointer)
{
    const int r = engine->RegisterGlobalProperty(declaration, pointer);
    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = wxT("Error while registering global variable.");
        status = EngineStatus::Error;
        return false;
    }

    return true;
}

bool AngelscriptRenderEngine::CompileFromPath(std::string path)
{
    const int r = CompileScriptFromPath(engine, std::move(path));
    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = wxT("Compile error.");
        status = EngineStatus::Error;
        return false;
    }
    return true;
}

bool AngelscriptRenderEngine::Execute()
{
    ctx = engine->CreateContext();
    if (ctx == nullptr)
    {
        errorInfo = wxT("Failed to create the context.");
        engine->Release();
        status = EngineStatus::Error;
        return false;
    }

    asIScriptFunction* renderFunc = engine->GetModule(nullptr)->GetFunctionByDecl("void Render()");
    if (renderFunc == nullptr)
    {
        errorInfo = wxT("The function 'Render' was not found.");
        ctx->Release();
        engine->Release();
        status = EngineStatus::Error;
        engine = nullptr;
        ctx = nullptr;
        return false;
    }

    int r = ctx->Prepare(renderFunc);
    if (r < 0)
    {
        errorInfo = wxT("Failed to prepare the context.");
        status = EngineStatus::Error;
        ctx->Release();
        engine->Release();
        return false;
    }
    ctx->Execute();

    ctx->Release();
    engine->Release();
    engine = nullptr;
    ctx = nullptr;
    asThreadCleanup();

    return true;
}

void AngelscriptRenderEngine::Abort()
{
    if (ctx != nullptr)
        ctx->Abort();
}

EngineStatus AngelscriptRenderEngine::GetStatus()
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
    else
        return errorInfo;
}
