#include <doctest/doctest.h>

#include "TextUtils.h"

TEST_CASE("Long long values are converted to wxString without truncation")
{
    CHECK(TextUtils::ToWxString(5'000'000'000LL) == "5000000000");
    CHECK(TextUtils::ToWxString(-5'000'000'000LL) == "-5000000000");
}
