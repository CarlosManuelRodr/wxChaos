#include <type_traits>
#include <doctest/doctest.h>
#include "FractalFactory.h"
#include "../../wxChaos/core/fractals/vector/ApollonianGasket.h"
#include "../../wxChaos/core/fractals/vector/VectorSierpinskiTriangle.h"
#include "../../wxChaos/core/vector/VectorFractal.h"

static_assert(std::is_base_of_v<VectorFractal, VectorSierpinskiTriangle>);
static_assert(std::is_base_of_v<VectorFractal, ApollonianGasket>);

TEST_CASE("Vector Sierpinski triangle draws the outer boundary at iteration zero")
{
    VectorSierpinskiTriangle triangle(640, 480);
    triangle.SetIterations(0);
    triangle.RenderBlocking();

    CHECK(triangle.GetLines().size() == 3);
    CHECK(triangle.GetSetMap() == nullptr);
}

TEST_CASE("Vector Sierpinski triangle adds one hole per surviving triangle")
{
    VectorSierpinskiTriangle triangle(640, 480);
    triangle.SetView({-1.5, -1.0, 1.5, 1.5});
    triangle.SetIterations(2);
    triangle.RenderBlocking();

    CHECK(triangle.GetLines().size() == 15);
}

TEST_CASE("Vector Sierpinski triangle prunes triangles outside the viewport")
{
    VectorSierpinskiTriangle triangle(640, 480);
    triangle.SetIterations(20);
    triangle.SetView({10.0, 10.0, 11.0, 11.0});
    triangle.RenderBlocking();

    CHECK(triangle.GetLines().empty());
    CHECK(triangle.GetRenderProgress() == 100);
}

TEST_CASE("Apollonian gasket starts with a Descartes configuration")
{
    ApollonianGasket gasket(640, 480);
    gasket.SetIterations(0);
    gasket.RenderBlocking();

    REQUIRE(gasket.GetCircles().size() == 4);
    CHECK(gasket.GetCircles().front().radius == doctest::Approx(1.0));
    CHECK(gasket.GetSetMap() == nullptr);
}

TEST_CASE("Apollonian gasket fills all four initial gaps")
{
    ApollonianGasket gasket(640, 480);
    gasket.SetIterations(1);
    gasket.RenderBlocking();

    CHECK(gasket.GetCircles().size() == 8);
}

TEST_CASE("Apollonian gasket uses the interior set color")
{
    ApollonianGasket gasket(640, 480);
    const sf::Color expected(12, 34, 56);
    gasket.SetFractalSetColor(expected);
    gasket.SetIterations(1);
    gasket.RenderBlocking();

    REQUIRE_FALSE(gasket.GetCircles().empty());
    CHECK(gasket.GetCircles().front().color == expected);
}

TEST_CASE("Apollonian gasket prunes recursive gaps outside the viewport")
{
    ApollonianGasket gasket(640, 480);
    gasket.SetIterations(20);
    gasket.SetView({10.0, 10.0, 11.0, 11.0});
    gasket.RenderBlocking();

    CHECK(gasket.GetCircles().empty());
    CHECK(gasket.GetRenderProgress() == 100);
}

TEST_CASE("Fractal factory creates both new vector fractals")
{
    FractalFactory factory;
    factory.CreateFractal(FractalType::VectorSierpinskiTriangle, 320, 240);
    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->IsVectorFractal());

    factory.CreateFractal(FractalType::ApollonianGasket, 320, 240);
    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->GetType() == FractalType::ApollonianGasket);
    CHECK(factory.GetFractal()->IsVectorFractal());
}
