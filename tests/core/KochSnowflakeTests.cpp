#include <type_traits>
#include <doctest/doctest.h>
#include "FractalFactory.h"
#include "../../wxChaos/core/raster/RasterFractal.h"
#include "../../wxChaos/core/raster/RasterRenderWorker.h"
#include "RenderWorker.h"
#include "../../wxChaos/core/vector/VectorFractal.h"
#include "VectorRenderWorker.h"
#include "fractals/KochSnowflake.h"
#include "fractals/Mandelbrot.h"

static_assert(std::is_base_of_v<RasterFractal, Mandelbrot>);
static_assert(std::is_base_of_v<VectorFractal, KochSnowflake>);
static_assert(std::is_base_of_v<RenderWorker, RasterRenderWorker>);
static_assert(std::is_base_of_v<RenderWorker, VectorRenderWorker>);
static_assert(!std::is_base_of_v<RasterRenderWorker, VectorRenderWorker>);

TEST_CASE("Koch snowflake creates three sides at iteration zero")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(0);
    snowflake.RenderBlocking();

    CHECK(snowflake.GetLines().size() == 3);
    CHECK(snowflake.GetSetMap() == nullptr);
}

TEST_CASE("Koch snowflake replaces every segment with four segments")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(3);
    snowflake.SetView({-1.5, -1.5, 1.5, 1.5});
    snowflake.RenderBlocking();

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
    snowflake.RenderBlocking();

    REQUIRE_FALSE(snowflake.GetLines().empty());
    CHECK(snowflake.GetLines().front().color == expected);
}

TEST_CASE("Koch snowflake limits geometry to visible screen detail")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(12);
    snowflake.RenderBlocking();

    CHECK_FALSE(snowflake.GetLines().empty());
    CHECK(snowflake.GetLines().size() < 3 * 4 * 4 * 4 * 4 * 4 * 4 * 4 * 4);
}

TEST_CASE("Koch snowflake prunes recursive branches outside the viewport")
{
    KochSnowflake snowflake(640, 480);
    snowflake.SetIterations(20);
    snowflake.SetView({10.0, 10.0, 11.0, 11.0});
    snowflake.RenderBlocking();

    CHECK(snowflake.GetLines().empty());
    CHECK(snowflake.GetRenderProgress() == 100);
}

TEST_CASE("Koch snowflake reports an asynchronous render")
{
    KochSnowflake snowflake(10000, 10000);
    snowflake.SetIterations(20);
    snowflake.MarkRenderStarted();
    snowflake.PrepareRender();
    snowflake.Render();

    CHECK(snowflake.IsRenderStarted());
    CHECK(snowflake.IsRendering());
    CHECK(snowflake.GetRenderProgress() >= 0);
    CHECK(snowflake.GetRenderProgress() <= 100);
    CHECK(snowflake.StopRender());
}

TEST_CASE("Fractal factory creates a Koch snowflake")
{
    FractalFactory factory;
    factory.CreateFractal(FractalType::KochSnowflake, 320, 240);

    REQUIRE(factory.GetFractal() != nullptr);
    CHECK(factory.GetFractal()->GetType() == FractalType::KochSnowflake);
    CHECK(factory.GetFractal()->IsVectorFractal());
}
