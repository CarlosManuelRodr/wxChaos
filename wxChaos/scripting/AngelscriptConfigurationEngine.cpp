// ReSharper disable CppExpressionWithoutSideEffects
#include "AngelscriptConfigurationEngine.h"
#include "AngelscriptBindings.h"
#include <scriptstdstring.h>

AngelscriptConfigurationEngine::AngelscriptConfigurationEngine()
{
    engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (engine == nullptr)
    {
        errorInfo = "Failed to create script engine.";
        status = EngineStatus::Error;
        return;
    }

    status = EngineStatus::Ok;
    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

    RegisterStdString(engine);
    RegisterScriptMathComplex(engine);
    RegisterScriptMathReal(engine);
    RegisterWxChaosInterface(engine);
    RegisterOrbitDrawingInterface(engine, nullptr);
    RegisterScriptOptionsInterface(engine, &configuration.options);

    int intVar;
    double dblVar;
    engine->RegisterGlobalProperty("double minX", &dblVar);
    engine->RegisterGlobalProperty("double maxX", &dblVar);
    engine->RegisterGlobalProperty("double minY", &dblVar);
    engine->RegisterGlobalProperty("double maxY", &dblVar);
    engine->RegisterGlobalProperty("double xFactor", &dblVar);
    engine->RegisterGlobalProperty("double yFactor", &dblVar);
    engine->RegisterGlobalProperty("double kReal", &dblVar);
    engine->RegisterGlobalProperty("double kImaginary", &dblVar);
    engine->RegisterGlobalProperty("double orbitX", &dblVar);
    engine->RegisterGlobalProperty("double orbitY", &dblVar);
    engine->RegisterGlobalProperty("int ho", &intVar);
    engine->RegisterGlobalProperty("int hf", &intVar);
    engine->RegisterGlobalProperty("int wo", &intVar);
    engine->RegisterGlobalProperty("int wf", &intVar);
    engine->RegisterGlobalProperty("int maxIter", &intVar);
    engine->RegisterGlobalProperty("int threadIndex", &intVar);
    engine->RegisterGlobalProperty("int screenWidth", &intVar);
    engine->RegisterGlobalProperty("int screenHeight", &intVar);
    engine->RegisterGlobalProperty("int paletteSize", &intVar);
}

AngelscriptConfigurationEngine::~AngelscriptConfigurationEngine()
{
    if (engine != nullptr)
    {
        engine->Release();
        engine = nullptr;
    }
}

bool AngelscriptConfigurationEngine::CompileFromPath(const std::string& path)
{
    const int r = CompileScriptFromPath(engine, path);
    filePath = path;

    if (r < 0)
    {
        engine->Release();
        engine = nullptr;
        errorInfo = "Compile error in file: " + path;
        status = EngineStatus::Error;
        return false;
    }
    return true;
}

bool AngelscriptConfigurationEngine::Execute()
{
    asIScriptContext* ctx = engine->CreateContext();
    if (ctx == nullptr)
    {
        errorInfo = "Error creating script context.";
        status = EngineStatus::Error;
        engine->Release();
        engine = nullptr;
        return false;
    }

    asIScriptModule* module = engine->GetModule(nullptr);
    asIScriptFunction* renderFunc = module->GetFunctionByDecl("void Configure()");
    if (renderFunc == nullptr)
    {
        errorInfo = "Couldn't find the Configure() function.";
        status = EngineStatus::Error;
        ctx->Release();
        engine->Release();
        engine = nullptr;
        return false;
    }

    if (const int r = ctx->Prepare(renderFunc); r < 0)
    {
        errorInfo = "Error while preparing the script context.";
        status = EngineStatus::Error;
        ctx->Release();
        engine->Release();
        engine = nullptr;
        return false;
    }

    ctx->Execute();
    const bool hasOrbit = module->GetFunctionByDecl("void DrawOrbit()") != nullptr;
    ctx->Release();
    engine->Release();
    engine = nullptr;
    ScriptOptions options = std::move(configuration.options);
    configuration = FetchScriptData(filePath);
    configuration.options = std::move(options);
    configuration.hasOrbit = hasOrbit;
    return true;
}

ScriptData AngelscriptConfigurationEngine::GetScriptData()
{
    return configuration;
}

EngineStatus AngelscriptConfigurationEngine::GetStatus() const
{
    return status;
}

wxString AngelscriptConfigurationEngine::GetErrorInfo()
{
    if (thereIsConsoleText)
    {
        thereIsConsoleText = false;
        return errorInfo + consoleText;
    }
    return errorInfo;
}
