#include <doctest/doctest.h>

#include "../../wxChaos/core/renderers/raster/utils/NewtonRootRegistry.h"

TEST_CASE("Newton root registry reuses nearby roots")
{
    NewtonRootRegistry registry(1e-5);

    const unsigned int firstId = registry.ClassifyOrAdd({1.0, 0.0});
    const unsigned int nearbyId = registry.ClassifyOrAdd({1.0 + 5e-6, 2e-6});

    CHECK(firstId == 0);
    CHECK(nearbyId == firstId);
    CHECK(registry.GetRoots().size() == 1);
}

TEST_CASE("Newton root registry adds distinct roots")
{
    NewtonRootRegistry registry(1e-5);

    CHECK(registry.ClassifyOrAdd({1.0, 0.0}) == 0);
    CHECK(registry.ClassifyOrAdd({-0.5, 0.8660254038}) == 1);
    CHECK(registry.ClassifyOrAdd({-0.5, -0.8660254038}) == 2);
    CHECK(registry.GetRoots().size() == 3);
}
