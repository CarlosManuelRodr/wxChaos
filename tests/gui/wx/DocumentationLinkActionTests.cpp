#include <doctest/doctest.h>

#include "wx/docs/DocumentationLinkAction.h"

TEST_CASE("DocumentationLinkAction parses known fractal actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://fractal/mandelbrot");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenFractal);
    CHECK(action.GetTarget() == "mandelbrot");
}

TEST_CASE("DocumentationLinkAction parses Mandelbrot landmarks")
{
    const DocumentationLinkAction action =
        DocumentationLinkAction::Parse("wxchaos://location/mandelbrot/seahorse-valley");

    CHECK(action.GetType() == DocumentationLinkAction::Type::OpenLocation);
    CHECK(action.GetLocation().fractal == "mandelbrot");
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

TEST_CASE("DocumentationLinkAction rejects unknown actions")
{
    const DocumentationLinkAction action = DocumentationLinkAction::Parse("wxchaos://location/mandelbrot/unknown");

    CHECK(action.GetType() == DocumentationLinkAction::Type::Unknown);
}
