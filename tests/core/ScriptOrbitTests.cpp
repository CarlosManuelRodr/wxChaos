#include <doctest/doctest.h>
#include <filesystem>
#include "AngelscriptConfigurationEngine.h"
#include "fractals/ScriptFractal.h"

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
