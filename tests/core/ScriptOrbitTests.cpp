#include <doctest/doctest.h>
#include <filesystem>
#include "AngelscriptConfigurationEngine.h"
#include "AngelscriptRenderEngine.h"
#include "fractals/ScriptFractal.h"

TEST_CASE("a complete bundled script compiles in the render engine")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";
    const std::filesystem::path scriptPath = scriptsDirectory / "Newton4.as";
    REQUIRE(std::filesystem::exists(scriptPath));

    {
        AngelscriptRenderEngine renderEngine;
        double doubleValue = 0.0;
        int intValue = 0;
        unsigned int unsignedValue = 0;
        REQUIRE(renderEngine.RegisterGlobalVariable("double minX", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double maxX", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double minY", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double maxY", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double xFactor", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double yFactor", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double kReal", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double kImaginary", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double orbitX", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double orbitY", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("int ho", &intValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("int hf", &intValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("int wo", &intValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("int wf", &intValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("double maxIter", &doubleValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("uint threadIndex", &unsignedValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("uint screenWidth", &unsignedValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("uint screenHeight", &unsignedValue));
        REQUIRE(renderEngine.RegisterGlobalVariable("uint paletteSize", &unsignedValue));
        REQUIRE(renderEngine.CompileFromPath(scriptPath.string()));
    }
}

TEST_CASE("bundled scripts compile with the orbit drawing interface")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";
    REQUIRE(std::filesystem::exists(scriptsDirectory));

    for (const std::filesystem::directory_entry& script : std::filesystem::directory_iterator(scriptsDirectory))
    {
        if (script.path().extension() != ".as")
            continue;

        AngelscriptConfigurationEngine engine;
        CAPTURE(script.path().string());
        REQUIRE(engine.CompileFromPath(script.path().string()));
        REQUIRE(engine.Execute());
    }
}

TEST_CASE("a complete bundled script executes its orbit entry point")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";
    const std::filesystem::path scriptPath = scriptsDirectory / "Newton4.as";
    REQUIRE(std::filesystem::exists(scriptPath));

    ScriptFractal fractal(32, 32, scriptPath.string());
    fractal.SetOrbitPoint(1.5, 0.25);
    fractal.DrawOrbit();

    CHECK(fractal.GetOrbitErrorInfo().empty());
    CHECK_FALSE(fractal.GetOrbitLines().empty());
}
