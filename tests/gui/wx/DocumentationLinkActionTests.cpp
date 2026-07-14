#include <doctest/doctest.h>

#include "wx/docs/DocumentationLinkAction.h"

TEST_CASE("DocumentationLinkAction parses known fractal actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://fractal/mandelbrot");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenFractal);
    CHECK(action.GetTarget() == "mandelbrot");
    CHECK(action.GetTargetFractalType() == FractalType::Mandelbrot);
    CHECK(action.TargetFractalEnablesJulia());
}

TEST_CASE("DocumentationLinkAction parses Logistic Map actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://fractal/logistic-map");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenFractal);
    CHECK(action.GetTarget() == "logistic-map");
    CHECK(action.GetTargetFractalType() == FractalType::LogisticMap);
    CHECK_FALSE(action.TargetFractalEnablesJulia());
}

TEST_CASE("DocumentationLinkAction parses vector fractal actions")
{
    const DocumentationLinkAction triangle =
        DocumentationLinkAction::Parse("wxchaos://fractal/vector-sierpinski-triangle");
    const DocumentationLinkAction gasket =
        DocumentationLinkAction::Parse("wxchaos://fractal/apollonian-gasket");

    CHECK(triangle.GetType() == DocumentationLinkAction::Type::OpenFractal);
    CHECK(triangle.GetTargetFractalType() == FractalType::VectorSierpinskiTriangle);
    CHECK(gasket.GetType() == DocumentationLinkAction::Type::OpenFractal);
    CHECK(gasket.GetTargetFractalType() == FractalType::ApollonianGasket);
}

TEST_CASE("DocumentationLinkAction parses Julia mode actions for compatible fractals")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://julia/mandelbrot");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenJuliaMode);
    CHECK(action.GetTarget() == "mandelbrot");
    CHECK(action.GetTargetFractalType() == FractalType::Mandelbrot);
    CHECK(action.TargetFractalEnablesJulia());
}

TEST_CASE("DocumentationLinkAction rejects Julia mode actions for incompatible fractals")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://julia/tricorn");

    CHECK(action.GetType() == DocumentationLinkAction::Type::Unknown);
}

TEST_CASE("DocumentationLinkAction parses Mandelbrot landmarks")
{
    const DocumentationLinkAction action =
        DocumentationLinkAction::Parse("wxchaos://location/mandelbrot/seahorse-valley");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenLocation);
    CHECK(action.GetLocation().fractal == "mandelbrot");
    CHECK(action.GetLocation().fractalType == FractalType::Mandelbrot);
    CHECK(action.GetLocation().enableJulia);
    CHECK(action.GetLocation().id == "seahorse-valley");
    CHECK(action.GetLocation().centerX == doctest::Approx(-0.7453));
    CHECK(action.GetLocation().centerY == doctest::Approx(0.1127));
    CHECK(action.GetLocation().radius == doctest::Approx(0.04));
}

TEST_CASE("DocumentationLinkAction parses known tool actions")
{
    const DocumentationLinkAction orbit = DocumentationLinkAction::Parse("wxchaos://tool/orbit");
    const DocumentationLinkAction juliaConstantSlider =
        DocumentationLinkAction::Parse("wxchaos://tool/julia-constant-slider");

    CHECK(orbit.GetType() == DocumentationLinkAction::Type::ToggleTool);
    CHECK(orbit.GetTarget() == "orbit");

    CHECK(juliaConstantSlider.GetType() == DocumentationLinkAction::Type::ToggleTool);
    CHECK(juliaConstantSlider.GetTarget() == "julia-constant-slider");
}

TEST_CASE("DocumentationLinkAction parses formula dialog actions")
{
    const DocumentationLinkAction escapeTime =
        DocumentationLinkAction::Parse("wxchaos://formula/user-defined-escape-time");
    const DocumentationLinkAction fixedPoint =
        DocumentationLinkAction::Parse("wxchaos://formula/user-defined-fixed-point");
    const DocumentationLinkAction newton =
        DocumentationLinkAction::Parse("wxchaos://formula/user-defined-newton");

    CHECK(escapeTime.GetType() == DocumentationLinkAction::Type::OpenFormulaDialog);
    CHECK(escapeTime.GetTargetFractalType() == FractalType::UserDefinedEscapeTime);
    CHECK(escapeTime.GetTargetFormulaType() == FormulaType::Complex);

    CHECK(fixedPoint.GetType() == DocumentationLinkAction::Type::OpenFormulaDialog);
    CHECK(fixedPoint.GetTargetFractalType() == FractalType::UserDefinedFixedPoint);
    CHECK(fixedPoint.GetTargetFormulaType() == FormulaType::FixedPoint);

    CHECK(newton.GetType() == DocumentationLinkAction::Type::OpenFormulaDialog);
    CHECK(newton.GetTargetFractalType() == FractalType::UserDefinedNewtonRaphson);
    CHECK(newton.GetTargetFormulaType() == FormulaType::NewtonRaphson);
}

TEST_CASE("DocumentationLinkAction parses fractal option actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://options/mandelbrot-zn");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenFractalOptions);
    CHECK(action.GetTarget() == "mandelbrot-zn");
    CHECK(action.GetTargetFractalType() == FractalType::MandelbrotZN);
    CHECK(action.TargetFractalEnablesJulia());
}

TEST_CASE("DocumentationLinkAction parses Mandelbrot rendering methods")
{
    const DocumentationLinkAction action =
        DocumentationLinkAction::Parse("wxchaos://rendering/mandelbrot/triangle-inequality");

    CHECK(action.GetType() == DocumentationLinkAction::Type::SetRendering);
    CHECK(action.GetRenderingMethod().fractal == "mandelbrot");
    CHECK(action.GetRenderingMethod().fractalType == FractalType::Mandelbrot);
    CHECK(action.GetRenderingMethod().enableJulia);
    CHECK(action.GetRenderingMethod().id == "triangle-inequality");
    CHECK(action.GetRenderingMethod().algorithm == RenderingAlgorithmType::TriangleInequality);
    CHECK_FALSE(action.GetRenderingMethod().smoothRender);
    CHECK_FALSE(action.GetRenderingMethod().orbitTrap);
}

TEST_CASE("DocumentationLinkAction parses Mandelbrot render feature methods")
{
    const DocumentationLinkAction smooth =
        DocumentationLinkAction::Parse("wxchaos://rendering/mandelbrot/smooth-coloring");
    const DocumentationLinkAction orbitTraps =
        DocumentationLinkAction::Parse("wxchaos://rendering/mandelbrot/orbit-traps");

    CHECK(smooth.GetType() == DocumentationLinkAction::Type::SetRendering);
    CHECK(smooth.GetRenderingMethod().algorithm == RenderingAlgorithmType::EscapeTime);
    CHECK(smooth.GetRenderingMethod().smoothRender);
    CHECK_FALSE(smooth.GetRenderingMethod().orbitTrap);

    CHECK(orbitTraps.GetType() == DocumentationLinkAction::Type::SetRendering);
    CHECK(orbitTraps.GetRenderingMethod().algorithm == RenderingAlgorithmType::EscapeTime);
    CHECK_FALSE(orbitTraps.GetRenderingMethod().smoothRender);
    CHECK(orbitTraps.GetRenderingMethod().orbitTrap);
}

TEST_CASE("DocumentationLinkAction uses a shared rendering method vocabulary")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://rendering/tricorn/escape-angle");

    CHECK(action.GetType() == DocumentationLinkAction::Type::SetRendering);
    CHECK(action.GetRenderingMethod().fractal == "tricorn");
    CHECK(action.GetRenderingMethod().fractalType == FractalType::Tricorn);
    CHECK_FALSE(action.GetRenderingMethod().enableJulia);
    CHECK(action.GetRenderingMethod().id == "escape-angle");
    CHECK(action.GetRenderingMethod().algorithm == RenderingAlgorithmType::EscapeAngle);
}

TEST_CASE("DocumentationLinkAction parses Logistic Map rendering methods")
{
    const DocumentationLinkAction chaoticMap =
        DocumentationLinkAction::Parse("wxchaos://rendering/logistic-map/chaotic-map");

    CHECK(chaoticMap.GetType() == DocumentationLinkAction::Type::SetRendering);
    CHECK(chaoticMap.GetRenderingMethod().fractalType == FractalType::LogisticMap);
    CHECK(chaoticMap.GetRenderingMethod().algorithm == RenderingAlgorithmType::ChaoticMap);
}

TEST_CASE("DocumentationLinkAction rejects unknown actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://location/mandelbrot/unknown");

    CHECK(action.GetType() == DocumentationLinkAction::Type::Unknown);
}
