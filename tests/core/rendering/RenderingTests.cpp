#include <doctest/doctest.h>
#include "coloring/PaletteMapping.h"
#include "rendering/RenderJob.h"
#include "rendering/RenderRegion.h"

TEST_CASE("RenderRegion default constructor creates an empty region")
{
    const RenderRegion region;

    CHECK(region.GetLeft() == 0);
    CHECK(region.GetTop() == 0);
    CHECK(region.GetRight() == 0);
    CHECK(region.GetBottom() == 0);
    CHECK(region.GetWidth() == 0);
    CHECK(region.GetHeight() == 0);
    CHECK(region.GetArea() == 0);
    CHECK(region.IsEmpty());
}

TEST_CASE("RenderRegion reports half-open dimensions")
{
    const RenderRegion region(2, 3, 12, 8);

    CHECK(region.GetLeft() == 2);
    CHECK(region.GetTop() == 3);
    CHECK(region.GetRight() == 12);
    CHECK(region.GetBottom() == 8);
    CHECK(region.GetWidth() == 10);
    CHECK(region.GetHeight() == 5);
    CHECK(region.GetArea() == 50);
    CHECK_FALSE(region.IsEmpty());
}

TEST_CASE("RenderRegion treats inverted bounds as empty")
{
    CHECK(RenderRegion(5, 0, 5, 10).IsEmpty());
    CHECK(RenderRegion(6, 0, 5, 10).IsEmpty());
    CHECK(RenderRegion(0, 3, 10, 3).IsEmpty());
    CHECK(RenderRegion(0, 4, 10, 3).IsEmpty());

    CHECK(RenderRegion(6, 0, 5, 10).GetArea() == 0);
    CHECK(RenderRegion(0, 4, 10, 3).GetArea() == 0);
}

TEST_CASE("RenderJob defaults to no work")
{
    const RenderJob job;

    CHECK(job.IsEmpty());
    CHECK(job.GetProgressOriginY() == 0);
}

TEST_CASE("RenderJob uses region top as the default progress origin")
{
    const RenderJob job(RenderRegion(4, 9, 14, 13));

    CHECK_FALSE(job.IsEmpty());
    CHECK(job.GetRegion().GetLeft() == 4);
    CHECK(job.GetRegion().GetTop() == 9);
    CHECK(job.GetRegion().GetRight() == 14);
    CHECK(job.GetRegion().GetBottom() == 13);
    CHECK(job.GetProgressOriginY() == 9);
}

TEST_CASE("RenderJob can override progress origin")
{
    const RenderJob job(RenderRegion(4, 9, 14, 13), 2);

    CHECK_FALSE(job.IsEmpty());
    CHECK(job.GetProgressOriginY() == 2);
}

TEST_CASE("PaletteMapping keeps linear mapping compatible")
{
    CHECK(PaletteMapping::Map(36.0, 0.0, 100.0, 300, 72.0,
                              PaletteMappingMode::Linear, 1.5, false) == doctest::Approx(150.0));
}

TEST_CASE("PaletteMapping exponential mode reshapes normalized values")
{
    const double mapped = PaletteMapping::Map(25.0, 0.0, 100.0, 300, 100.0,
                                             PaletteMappingMode::Exponential, 2.0, false);

    CHECK(mapped == doctest::Approx(18.75));
}

TEST_CASE("PaletteMapping exponential relative mode spans the palette once")
{
    const double mapped = PaletteMapping::Map(5.0, 0.0, 10.0, 101, 72.0,
                                             PaletteMappingMode::Exponential, 2.0, true);

    CHECK(mapped == doctest::Approx(25.0));
}
