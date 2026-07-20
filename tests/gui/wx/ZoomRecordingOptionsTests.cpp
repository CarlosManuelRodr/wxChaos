#include <doctest/doctest.h>
#include "export/ZoomRecordingOptions.h"

TEST_CASE("Zoom recording defaults to 2x anti-aliasing")
{
    const ZoomRecordingOptions options;
    CHECK(options.antiAliasingScale == 2U);
}
