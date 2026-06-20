#pragma once

#include "../geometry/Rect.h"
#include "HighPrecision.h"

struct PreciseRect
{
    HighPrecisionReal left;
    HighPrecisionReal bottom;
    HighPrecisionReal right;
    HighPrecisionReal top;

    PreciseRect() : left(0), bottom(0), right(0), top(0) {}

    PreciseRect(const HighPrecisionReal& leftValue, const HighPrecisionReal& bottomValue,
                const HighPrecisionReal& rightValue, const HighPrecisionReal& topValue)
        : left(leftValue), bottom(bottomValue), right(rightValue), top(topValue)
    {
    }

    explicit PreciseRect(const Rect& view)
        : left(view._left), bottom(view._bottom), right(view._right), top(view._top)
    {
    }

    [[nodiscard]] Rect ToRect() const
    {
        return {ToDouble(left), ToDouble(bottom), ToDouble(right), ToDouble(top)};
    }
};
