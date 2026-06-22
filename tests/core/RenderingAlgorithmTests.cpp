#include <doctest/doctest.h>
#include "coloring/RenderingAlgorithm.h"

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
