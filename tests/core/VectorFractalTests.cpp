#include <type_traits>
#include <doctest/doctest.h>
#include "FractalFactory.h"
#include "../../wxChaos/core/fractals/vector/VectorSierpinskiTriangle.h"
#include "../../wxChaos/core/vector/VectorFractal.h"

static_assert(std::is_base_of_v<VectorFractal, VectorSierpinskiTriangle>);

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

TEST_CASE("Fractal factory creates the vector Sierpinski triangle")
{
    FractalFactory factory;
    factory.CreateFractal(FractalType::VectorSierpinskiTriangle, 320, 240);
    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->IsVectorFractal());
    CHECK_FALSE(factory.GetFractal()->SupportsColorRotation());
}
