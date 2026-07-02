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
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://tool/orbit");

    CHECK(action.GetType() == DocumentationLinkAction::Type::EnableTool);
    CHECK(action.GetTarget() == "orbit");
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

TEST_CASE("DocumentationLinkAction rejects unknown actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://location/mandelbrot/unknown");

    CHECK(action.GetType() == DocumentationLinkAction::Type::Unknown);
}
