#include <doctest/doctest.h>
#include "analysis/BoxCountMap.h"
#include "analysis/BoxCountWorker.h"
#include "fractals/raster/Mandelbrot.h"
#include "fractals/vector/KochSnowflake.h"
#include "fractals/vector/SierpinskiCarpet.h"

TEST_CASE("Box-count occupancy preserves raster set membership")
{
    Mandelbrot mandelbrot(64, 64);
    mandelbrot.RenderBlocking();

    BoxCountMap map;
    map.Build(mandelbrot);

    CHECK(map.GetSize() == 64);
    CHECK(map.GetOccupiedPixelCount() > 0);
    CHECK(map.GetOccupiedPixelCount() < 64 * 64);
}

TEST_CASE("Box-count occupancy rasterizes vector fractal lines")
{
    KochSnowflake snowflake(120, 120);
    snowflake.SetIterations(0);
    snowflake.RenderBlocking();

    BoxCountMap map;
    map.Build(snowflake);

    CHECK(map.GetSize() == 120);
    CHECK(map.GetOccupiedPixelCount() >= 100);
}

TEST_CASE("Box-count worker detects boxes crossed by vector lines")
{
    KochSnowflake snowflake(120, 120);
    snowflake.SetIterations(0);
    snowflake.RenderBlocking();

    BoxCountMap map;
    map.Build(snowflake);

    BoxCountWorker worker;
    worker.SetMap(&map, 0, map.GetSize());
    worker.SetDiv(12);
    worker.Run();

    CHECK(worker.GetBoxCount() > 0);
    CHECK(worker.GetBoxCount() <= 12 * 12);
}

TEST_CASE("Box-count occupancy clears Sierpinski carpet holes")
{
    SierpinskiCarpet carpet(90, 90);
    carpet.SetView({-1.0, -1.0, 1.0, 1.0});
    carpet.SetIterations(1);
    carpet.RenderBlocking();

    BoxCountMap map;
    map.Build(carpet);

    CHECK(map.IsOccupied(5, 5));
    CHECK_FALSE(map.IsOccupied(45, 45));
    CHECK(map.GetOccupiedPixelCount() > 0);
    CHECK(map.GetOccupiedPixelCount() < 90 * 90);
}
