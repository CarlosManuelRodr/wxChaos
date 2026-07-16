#include <doctest/doctest.h>

#include "SystemUtilities.h"

TEST_CASE("Processor count is always usable by render workers")
{
    CHECK(Platform::ProcessorCount() >= 1);
}
