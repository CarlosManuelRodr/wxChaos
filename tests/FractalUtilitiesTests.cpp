#include <cmath>
#include <doctest/doctest.h>
#include "FractalUtilities.h"

TEST_CASE("minVal returns the smaller value")
{
    CHECK(minVal(3.0, 5.0) == doctest::Approx(3.0));
    CHECK(minVal(5.0, 3.0) == doctest::Approx(3.0));
    CHECK(minVal(-2.0, -7.0) == doctest::Approx(-7.0));
}

TEST_CASE("gaussianIntDist measures distance to the nearest Gaussian integer")
{
    const double distance = gaussianIntDist(0.25, -0.75);
    const double expected = std::sqrt(0.25 * 0.25 + 0.25 * 0.25);

    CHECK(distance == doctest::Approx(expected));
}

TEST_CASE("gaussianIntDist rounds half values upward")
{
    CHECK(gaussianIntDist(0.5, -0.5) == doctest::Approx(std::sqrt(0.5)));
}

TEST_CASE("TIA follows the triangle inequality average formula")
{
    const double value = TIA(3.0, 4.0, 1.0, 0.0, 2.0, 0.0);

    CHECK(value == doctest::Approx(2.0));
}
