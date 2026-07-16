#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include "AngelscriptConfigurationEngine.h"
#include "../../wxChaos/core/fractals/raster/ScriptFractal.h"

namespace
{
    std::filesystem::path BundledScriptsDirectory()
    {
        return std::filesystem::path(WXCHAOS_SOURCE_DIR) / "app_resources" / "Scripts";
    }
}

TEST_CASE("bundled scripts compile with the orbit drawing interface")
{
    const std::filesystem::path scriptsDirectory = BundledScriptsDirectory();
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

TEST_CASE("script configuration registers its coordinate system")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";

    AngelscriptConfigurationEngine engine;
    REQUIRE(engine.CompileFromPath((scriptsDirectory / "Duffing.as").string()));
    REQUIRE(engine.Execute());

    const ScriptData data = engine.GetScriptData();
    CHECK(data.horizontalCoordinate == "x");
    CHECK(data.verticalCoordinate == "y");
}

TEST_CASE("scripts explicitly opt into the dimension calculator with a preset")
{
    const std::filesystem::path scriptPath = std::filesystem::temp_directory_path()
                                           / "wxchaos_dimension_calculator_preset_test.as";
    {
        std::ofstream script(scriptPath);
        script << R"(
void Configure()
{
    SetFractalName("Dimension test");
    SetCategory("Other");
    SetDimensionCalculatorEnabled(true);
    SetDimensionCalculatorPreset(-1.25, 1.5, -0.75, 42, "7*x", 2, 80, 4096);
}

void Render() {}
)";
    }

    AngelscriptConfigurationEngine engine;
    REQUIRE(engine.CompileFromPath(scriptPath.string()));
    REQUIRE(engine.Execute());
    std::filesystem::remove(scriptPath);

    const ScriptData data = engine.GetScriptData();
    CHECK(data.dimensionCalculatorEnabled);
    REQUIRE(data.dimensionCalculatorPreset.has_value());
    CHECK(data.dimensionCalculatorPreset->minX == doctest::Approx(-1.25));
    CHECK(data.dimensionCalculatorPreset->maxX == doctest::Approx(1.5));
    CHECK(data.dimensionCalculatorPreset->minY == doctest::Approx(-0.75));
    CHECK(data.dimensionCalculatorPreset->iterations == 42);
    CHECK(data.dimensionCalculatorPreset->divisionFunction == "7*x");
    CHECK(data.dimensionCalculatorPreset->functionXMin == 2);
    CHECK(data.dimensionCalculatorPreset->functionXMax == 80);
    CHECK(data.dimensionCalculatorPreset->imageSize == 4096);
}

TEST_CASE("bundled scripts do not enter the dimension calculator implicitly")
{
    for (const char* scriptName : {"Pawn.as", "CutBill.as"})
    {
        AngelscriptConfigurationEngine engine;
        CAPTURE(scriptName);
        REQUIRE(engine.CompileFromPath((BundledScriptsDirectory() / scriptName).string()));
        REQUIRE(engine.Execute());

        const ScriptData data = engine.GetScriptData();
        CHECK_FALSE(data.dimensionCalculatorEnabled);
        CHECK_FALSE(data.dimensionCalculatorPreset.has_value());
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

TEST_CASE("script orbit capability follows the presence of DrawOrbit")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";

    ScriptFractal newton(32, 32, (scriptsDirectory / "Newton4.as").string());
    CHECK(newton.HasOrbit());

    for (const char* scriptName : {"Duffing.as", "CutBill.as", "Gauss.as"})
    {
        CAPTURE(scriptName);
        ScriptFractal fractal(32, 32, (scriptsDirectory / scriptName).string());
        CHECK_FALSE(fractal.HasOrbit());
    }
}

TEST_CASE("script-defined options are exposed through the fractal options panel")
{
    std::filesystem::path scriptsDirectory = std::filesystem::current_path() / "bin" / "Scripts";
    if (!std::filesystem::exists(scriptsDirectory))
        scriptsDirectory = std::filesystem::current_path() / "cmake-build-debug" / "bin" / "Scripts";

    ScriptFractal duffing(32, 32, (scriptsDirectory / "Duffing.as").string());
    REQUIRE(duffing.GetOptionsPanel()->GetElementsSize() == 1);
    CHECK(duffing.GetOptionsPanel()->GetPanelOptionType(0) == PanelOptionType::TextCtrl);
    CHECK(*duffing.GetOptionsPanel()->GetDoubleValue(0) == doctest::Approx(2.75));

    *duffing.GetOptionsPanel()->GetDoubleValue(0) = 3.25;
    CHECK(duffing.GetScriptData().options.GetDouble("a") == doctest::Approx(3.25));
    duffing.RenderBlocking();
    CHECK_FALSE(duffing.IsThereError());

    for (const char* scriptName : {"Newton4.as", "NewtonCos.as", "NewtonLog.as", "NewtonPol.as", "NewtonSin.as",
                                   "NewtonTan.as"})
    {
        CAPTURE(scriptName);
        ScriptFractal newton(32, 32, (scriptsDirectory / scriptName).string());
        REQUIRE(newton.GetOptionsPanel()->GetElementsSize() == 1);
        CHECK(newton.GetOptionsPanel()->GetLabelValue(0) == "Minimum step: ");
        CHECK(newton.GetScriptData().options.GetDouble("minStep") == doctest::Approx(0.01));
    }

    ScriptFractal newton(32, 32, (scriptsDirectory / "Newton4.as").string());
    *newton.GetOptionsPanel()->GetDoubleValue(0) = 100.0;
    newton.SetOrbitPoint(1.5, 0.25);
    newton.DrawOrbit();
    CHECK(newton.GetOrbitErrorInfo().empty());
    CHECK(newton.GetOrbitLines().size() == 1);
}
