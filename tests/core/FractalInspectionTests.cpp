#include <doctest/doctest.h>
#include <string>
#include "FractalFactory.h"
#include "fractals/LogisticMap.h"
#include "fractals/Mandelbrot.h"
#include "fractals/ScriptFractal.h"

namespace
{
    std::string ToString(const wxString& value)
    {
        return value.ToStdString();
    }

    std::string LineWithPrefix(const std::string& text, const std::string& prefix)
    {
        const std::size_t start = text.find(prefix);
        if (start == std::string::npos)
            return "";

        const std::size_t end = text.find('\n', start);
        return text.substr(start, end == std::string::npos ? std::string::npos : end - start);
    }

    std::string LyapunovLine(const LogisticMap& fractal, const double x, const double y)
    {
        return LineWithPrefix(ToString(fractal.InspectPoint(x, y, 100)), "Lyapunov exponent:");
    }

    class InspectableScriptFractal : public ScriptFractal
    {
    public:
        InspectableScriptFractal(const unsigned int width, const unsigned int height, const ScriptData& scriptData)
            : ScriptFractal(width, height, scriptData, 1)
        {
        }

        using ScriptFractal::CreateInspectionFractal;
    };
}

TEST_CASE("generic fractal point inspection uses neutral coordinates")
{
    Mandelbrot fractal(32, 32);

    const std::string output = ToString(fractal.InspectPoint(-0.75, 0.1, 20));

    CHECK(output.find("Fractal: Mandelbrot") != std::string::npos);
    CHECK(output.find("Coordinates: (") != std::string::npos);
    CHECK(output.find("Point:") == std::string::npos);
    CHECK(output.find("Algorithm: escape time") != std::string::npos);
    CHECK(output.find("Maximum iterations: 20") != std::string::npos);
    CHECK(output.find("Result:") != std::string::npos);
}

TEST_CASE("script fractal creates inspection probes through its override")
{
    ScriptData scriptData;
    scriptData.file = "inspection-test.as";
    InspectableScriptFractal fractal(8, 8, scriptData);

    FractalFactory factory;
    fractal.CreateInspectionFractal(factory, 3, 3);

    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->GetType() == FractalType::ScriptFractal);
}

TEST_CASE("Logistic map point inspection calculates Lyapunov exponent from x coordinate")
{
    LogisticMap fractal(100, 80);

    const std::string first = LyapunovLine(fractal, 3.2, 0.25);
    const std::string second = LyapunovLine(fractal, 3.8, 0.25);

    CHECK(first.find("Lyapunov exponent:") != std::string::npos);
    CHECK(second.find("Lyapunov exponent:") != std::string::npos);
    CHECK(first != second);
}

TEST_CASE("Logistic map point inspection ignores y coordinate for Lyapunov exponent")
{
    LogisticMap fractal(100, 80);

    const std::string lowerY = LyapunovLine(fractal, 3.7, 0.1);
    const std::string upperY = LyapunovLine(fractal, 3.7, 0.9);

    CHECK(lowerY == upperY);
}

TEST_CASE("Logistic map point inspection reports iteration override")
{
    LogisticMap fractal(100, 80);

    const std::string output = ToString(fractal.InspectPoint(3.7, 0.4, 42));

    CHECK(output.find("Maximum iterations: 42") != std::string::npos);
}

TEST_CASE("Logistic map point inspection honors stabilization option")
{
    LogisticMap stabilized(100, 80);
    LogisticMap raw(100, 80);
    *raw.GetOptionsPanel()->GetBoolValue(0) = false;
    raw.CopyOptionFromPanel();

    const std::string stabilizedLine = LyapunovLine(stabilized, 3.7, 0.4);
    const std::string rawLine = LyapunovLine(raw, 3.7, 0.4);

    CHECK(stabilizedLine != rawLine);
}

TEST_CASE("Logistic map point inspection classifies positive exponent as chaotic")
{
    LogisticMap fractal(100, 80);

    const std::string output = ToString(fractal.InspectPoint(4.0, 0.4, 100));

    CHECK(output.find("Lyapunov exponent:") != std::string::npos);
    CHECK(output.find("Result: chaotic") != std::string::npos);
}
