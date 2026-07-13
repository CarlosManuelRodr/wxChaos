#include <type_traits>
#include <doctest/doctest.h>
#include "FractalFactory.h"
#include "RasterFractal.h"
#include "VectorFractal.h"
#include "fractals/KochSnowflake.h"
#include "fractals/Mandelbrot.h"

static_assert(std::is_base_of_v<RasterFractal, Mandelbrot>);
static_assert(std::is_base_of_v<VectorFractal, KochSnowflake>);

TEST_CASE("Koch snowflake creates three sides at iteration zero")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(0);
    snowflake.PrepareRender();
    snowflake.Render();

    CHECK(snowflake.GetLines().size() == 3);
    CHECK(snowflake.GetSetMap() == nullptr);
}

TEST_CASE("Koch snowflake replaces every segment with four segments")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(3);
    snowflake.PrepareRender();
    snowflake.Render();

    CHECK(snowflake.GetLines().size() == 3 * 4 * 4 * 4);
}

TEST_CASE("Koch snowflake uses the shared zoomable world-coordinate view")
{
    KochSnowflake snowflake(640, 480);
    const int initialPixel = snowflake.GetPixelX(1.0);

    snowflake.SetView({-0.5, -0.5, 0.5, 0.5});

    CHECK(snowflake.GetPixelX(1.0) > initialPixel);
    CHECK_FALSE(snowflake.IsRendered());
}

TEST_CASE("Koch snowflake uses the interior set color")
{
    KochSnowflake snowflake(640, 480);
    const sf::Color expected(12, 34, 56);
    snowflake.SetFractalSetColor(expected);
    snowflake.SetIterations(0);
    snowflake.PrepareRender();
    snowflake.Render();

    REQUIRE_FALSE(snowflake.GetLines().empty());
    CHECK(snowflake.GetLines().front().color == expected);
}

TEST_CASE("Fractal factory creates a Koch snowflake")
{
    FractalFactory factory;
    factory.CreateFractal(FractalType::KochSnowflake, 320, 240);

    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->GetType() == FractalType::KochSnowflake);
    CHECK(factory.GetFractal()->IsVectorFractal());
}
