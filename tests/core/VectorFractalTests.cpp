#include <type_traits>
#include <cmath>
#include <doctest/doctest.h>
#include "FractalFactory.h"
#include "../../wxChaos/core/fractals/vector/VectorSierpinskiTriangle.h"
#include "../../wxChaos/core/fractals/vector/SierpinskiCarpet.h"
#include "../../wxChaos/core/vector/VectorFractal.h"
#include "../../wxChaos/core/numeric/HighPrecision.h"

static_assert(std::is_base_of_v<VectorFractal, VectorSierpinskiTriangle>);
static_assert(std::is_base_of_v<VectorFractal, SierpinskiCarpet>);

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

TEST_CASE("Sierpinski carpet starts as one occupied square")
{
    SierpinskiCarpet carpet(640, 480);
    carpet.SetIterations(0);
    carpet.RenderBlocking();

    REQUIRE(carpet.GetRectangles().size() == 1);
    CHECK(carpet.GetRectangles().front().belongsToSet);
    CHECK(carpet.GetSetMap() == nullptr);
}

TEST_CASE("Sierpinski carpet removes one center from every retained square")
{
    SierpinskiCarpet carpet(640, 480);
    carpet.SetView({-1.2, -1.2, 1.2, 1.2});
    carpet.SetIterations(2);
    carpet.RenderBlocking();

    REQUIRE(carpet.GetRectangles().size() == 10);
    CHECK(carpet.GetRectangles().front().belongsToSet);
    for (std::size_t i = 1; i < carpet.GetRectangles().size(); i++)
        CHECK_FALSE(carpet.GetRectangles()[i].belongsToSet);
}

TEST_CASE("Sierpinski carpet prunes squares outside the viewport")
{
    SierpinskiCarpet carpet(640, 480);
    carpet.SetIterations(20);
    carpet.SetView({10.0, 10.0, 11.0, 11.0});
    carpet.RenderBlocking();

    CHECK(carpet.GetRectangles().empty());
    CHECK(carpet.GetRenderProgress() == 100);
}

TEST_CASE("Fractal factory creates the Sierpinski carpet")
{
    FractalFactory factory;
    factory.CreateFractal(FractalType::SierpinskiCarpet, 320, 240);
    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->IsVectorFractal());
    CHECK(factory.GetFractal()->GetType() == FractalType::SierpinskiCarpet);
}

TEST_CASE("Vector Sierpinski triangle switches to precise screen geometry for deep zooms")
{
    HighPrecisionReal::PrecisionScope precision(256);
    HighPrecisionReal radius(1);
    for (int i = 0; i < 120; i++)
        radius /= HighPrecisionReal(2);
    const HighPrecisionReal top = HighPrecisionReal(2) / sqrt(HighPrecisionReal(3));

    VectorSierpinskiTriangle triangle(320, 240);
    triangle.SetPreciseView({-radius, top - radius, radius, top + radius});
    triangle.SetIterations(150);

    CHECK(triangle.IsHighPrecisionRenderActive());
    triangle.RenderBlocking();
    REQUIRE_FALSE(triangle.GetLines().empty());
    for (const LineData& line : triangle.GetLines())
    {
        CHECK(line.screenSpace);
        CHECK(std::isfinite(line.x1));
        CHECK(std::isfinite(line.y1));
        CHECK(std::isfinite(line.x2));
        CHECK(std::isfinite(line.y2));
    }
}

TEST_CASE("Sierpinski carpet preserves recursive holes beyond double precision")
{
    HighPrecisionReal::PrecisionScope precision(256);
    HighPrecisionReal radius(1);
    for (int i = 0; i < 120; i++)
        radius /= HighPrecisionReal(2);

    SierpinskiCarpet carpet(320, 240);
    carpet.SetPreciseView({HighPrecisionReal(-1) - radius, -radius,
                           HighPrecisionReal(-1) + radius, radius});
    carpet.SetIterations(150);

    CHECK(carpet.IsHighPrecisionRenderActive());
    carpet.RenderBlocking();
    REQUIRE(carpet.GetRectangles().size() > 1);
    for (const RectangleData& rectangle : carpet.GetRectangles())
    {
        CHECK(rectangle.screenSpace);
        CHECK(std::isfinite(rectangle.left));
        CHECK(std::isfinite(rectangle.right));
        CHECK(std::isfinite(rectangle.bottom));
        CHECK(std::isfinite(rectangle.top));
    }
}

TEST_CASE("Resizing interrupts a precise vector render and allows a replacement render")
{
    HighPrecisionReal::PrecisionScope precision(256);
    HighPrecisionReal radius(1);
    for (int i = 0; i < 120; i++)
        radius /= HighPrecisionReal(2);

    SierpinskiCarpet carpet(10000, 10000);
    carpet.SetPreciseView({HighPrecisionReal(-1) - radius, -radius,
                           HighPrecisionReal(-1) + radius, radius});
    carpet.SetIterations(150);
    carpet.MarkRenderStarted();
    carpet.PrepareRender({0, 0});
    carpet.Render();

    REQUIRE(carpet.IsRenderStarted());
    REQUIRE(carpet.IsRendering());
    carpet.Resize(640, 480);

    CHECK_FALSE(carpet.IsRenderStarted());
    CHECK_FALSE(carpet.IsRendering());
    CHECK_FALSE(carpet.IsRendered());
    carpet.RenderBlocking();
    CHECK(carpet.IsRendered());
    CHECK_FALSE(carpet.GetRectangles().empty());
}
