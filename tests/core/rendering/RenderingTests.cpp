#include <doctest/doctest.h>
#include "coloring/ColorPalette.h"
#include "coloring/PaletteMapping.h"
#include "numeric/HighPrecision.h"
#include "Options.h"
#include "rasterization/RasterJob.h"
#include "rasterization/RasterRegion.h"

TEST_CASE("RenderRegion default constructor creates an empty region")
{
    const RasterRegion region;

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
    const RasterRegion region(2, 3, 12, 8);

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
    CHECK(RasterRegion(5, 0, 5, 10).IsEmpty());
    CHECK(RasterRegion(6, 0, 5, 10).IsEmpty());
    CHECK(RasterRegion(0, 3, 10, 3).IsEmpty());
    CHECK(RasterRegion(0, 4, 10, 3).IsEmpty());

    CHECK(RasterRegion(6, 0, 5, 10).GetArea() == 0);
    CHECK(RasterRegion(0, 4, 10, 3).GetArea() == 0);
}

TEST_CASE("RenderJob defaults to no work")
{
    const RasterJob job;

    CHECK(job.IsEmpty());
    CHECK(job.GetProgressOriginY() == 0);
}

TEST_CASE("RenderJob uses region top as the default progress origin")
{
    const RasterJob job(RasterRegion(4, 9, 14, 13));

    CHECK_FALSE(job.IsEmpty());
    CHECK(job.GetRegion().GetLeft() == 4);
    CHECK(job.GetRegion().GetTop() == 9);
    CHECK(job.GetRegion().GetRight() == 14);
    CHECK(job.GetRegion().GetBottom() == 13);
    CHECK(job.GetProgressOriginY() == 9);
}

TEST_CASE("RenderJob can override progress origin")
{
    const RasterJob job(RasterRegion(4, 9, 14, 13), 2);

    CHECK_FALSE(job.IsEmpty());
    CHECK(job.GetProgressOriginY() == 2);
}

TEST_CASE("Anti-aliasing is off by default")
{
    const Options options;

    CHECK(options.antiAliasingScale == 1);
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

TEST_CASE("Recovered image-inspired palettes expose full cyclic gradients")
{
    ColorPalette coolBlue;
    coolBlue.SetStyle(CoolBlue);

    CHECK(coolBlue.paletteSize == 720);
    CHECK(coolBlue.colorCycleLength == 72);
    CHECK(coolBlue.grad.find("rgb(36,123,225)") != std::string::npos);
    CHECK(coolBlue.grad.find("rgb(233,173,116)") != std::string::npos);

    ColorPalette starryNight;
    starryNight.SetStyle(StarryNight);

    CHECK(starryNight.paletteSize == 720);
    CHECK(starryNight.colorCycleLength == 72);
    CHECK(starryNight.grad.find("rgb(40,48,144)") != std::string::npos);
    CHECK(starryNight.grad.find("rgb(249,243,186)") != std::string::npos);
}

TEST_CASE("Creative palettes expose harmonious cyclic gradients")
{
    ColorPalette moonlitGarden;
    moonlitGarden.SetStyle(MoonlitGarden);
    CHECK(moonlitGarden.paletteSize == 540);
    CHECK(moonlitGarden.colorCycleLength == 84);
    CHECK(moonlitGarden.grad.find("rgb(85,132,82)") != std::string::npos);
    CHECK(moonlitGarden.grad.find("rgb(230,217,157)") != std::string::npos);

    ColorPalette coralReef;
    coralReef.SetStyle(CoralReef);
    CHECK(coralReef.paletteSize == 560);
    CHECK(coralReef.colorCycleLength == 84);
    CHECK(coralReef.grad.find("rgb(0,135,143)") != std::string::npos);
    CHECK(coralReef.grad.find("rgb(244,151,118)") != std::string::npos);

    ColorPalette desertMirage;
    desertMirage.SetStyle(DesertMirage);
    CHECK(desertMirage.paletteSize == 600);
    CHECK(desertMirage.colorCycleLength == 96);
    CHECK(desertMirage.grad.find("rgb(46,43,82)") != std::string::npos);
    CHECK(desertMirage.grad.find("rgb(245,219,154)") != std::string::npos);

    ColorPalette polarEmber;
    polarEmber.SetStyle(PolarEmber);
    CHECK(polarEmber.paletteSize == 600);
    CHECK(polarEmber.colorCycleLength == 84);
    CHECK(polarEmber.grad.find("rgb(75,139,162)") != std::string::npos);
    CHECK(polarEmber.grad.find("rgb(223,151,93)") != std::string::npos);

    ColorPalette jadeTemple;
    jadeTemple.SetStyle(JadeTemple);
    CHECK(jadeTemple.paletteSize == 540);
    CHECK(jadeTemple.colorCycleLength == 84);
    CHECK(jadeTemple.grad.find("rgb(31,123,84)") != std::string::npos);
    CHECK(jadeTemple.grad.find("rgb(238,226,178)") != std::string::npos);
}

TEST_CASE("HighPrecisionReal formats compact and extended coordinate strings")
{
    CHECK(HighPrecisionReal(0.2).ToString(15) == "0.2");

    HighPrecisionReal::PrecisionScope precision(128);
    const HighPrecisionReal third = HighPrecisionReal(1) / HighPrecisionReal(3);
    const std::string formatted = third.ToString(30);

    CHECK(formatted.rfind("0.333333333333333333333333333333", 0) == 0);
}
