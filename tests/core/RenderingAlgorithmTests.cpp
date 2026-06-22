#include <doctest/doctest.h>
#include "RenderingAlgorithm.h"

TEST_CASE("Gaussian integer and Buddhabrot use relative color by default")
{
    CHECK(RenderingAlgorithm::UsesRelativeColorByDefault(RenderingAlgorithmType::GaussianInt));
    CHECK(RenderingAlgorithm::UsesRelativeColorByDefault(RenderingAlgorithmType::Buddhabrot));
}

TEST_CASE("Escape-time family algorithms do not use relative color by default")
{
    CHECK_FALSE(RenderingAlgorithm::UsesRelativeColorByDefault(RenderingAlgorithmType::EscapeTime));
    CHECK_FALSE(RenderingAlgorithm::UsesRelativeColorByDefault(RenderingAlgorithmType::EscapeAngle));
    CHECK_FALSE(RenderingAlgorithm::UsesRelativeColorByDefault(RenderingAlgorithmType::TriangleInequality));
}

TEST_CASE("Only algorithms with palette defaults override the palette size")
{
    CHECK(RenderingAlgorithm::GetDefaultPaletteSize(RenderingAlgorithmType::Buddhabrot, 300) == 1500);
    CHECK(RenderingAlgorithm::GetDefaultPaletteSize(RenderingAlgorithmType::GaussianInt, 300) == 300);
    CHECK(RenderingAlgorithm::GetDefaultPaletteSize(RenderingAlgorithmType::EscapeTime, 300) == 300);
}
