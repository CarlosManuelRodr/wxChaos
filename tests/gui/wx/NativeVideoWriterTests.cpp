#include <doctest/doctest.h>
#include "export/NativeVideoWriter.h"

TEST_CASE("Native video bitrate recommendation scales with resolution and frame rate")
{
    CHECK(NativeVideoWriter::GetRecommendedBitRate(1920, 1080, 30) == 31104000U);
    CHECK(NativeVideoWriter::GetRecommendedBitRate(1920, 1080, 60) == 62208000U);
}

TEST_CASE("Native video bitrate recommendation enforces its minimum")
{
    CHECK(NativeVideoWriter::GetRecommendedBitRate(320, 240, 24) == 1000000U);
}

TEST_CASE("Native video bitrate recommendation safely handles extreme dimensions")
{
    CHECK(NativeVideoWriter::GetRecommendedBitRate(16384, 16384, 120) == 4294967295U);
}
