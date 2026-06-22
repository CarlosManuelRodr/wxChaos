#include "PaletteMapping.h"

#include <algorithm>
#include <cmath>

double PaletteMapping::Map(const double value, const double minValue, const double maxValue,
                           const unsigned int paletteSize, const double cycleLength,
                           const PaletteMappingMode mode, const double exponent,
                           const bool relativeColor)
{
    if (paletteSize == 0)
        return 0.0;

    if (mode == PaletteMappingMode::Linear)
    {
        if (!relativeColor)
            return value * static_cast<double>(paletteSize) / std::max(1.0, cycleLength);

        const double ratio = std::clamp((value - minValue) / (maxValue - minValue), 0.0, 1.0);
        return ratio * static_cast<double>(paletteSize - 1);
    }

    const double normalizedValue = relativeColor
                                       ? (value - minValue) / (maxValue - minValue)
                                       : value / std::max(1.0, maxValue);
    const double normalized = std::clamp(normalizedValue, 0.0, 1.0);
    const double mapped = std::pow(normalized, std::max(0.0001, exponent));

    if (relativeColor)
        return mapped * static_cast<double>(paletteSize - 1);

    const double cycles = std::max(1.0, maxValue / std::max(1.0, cycleLength));
    return mapped * cycles * static_cast<double>(paletteSize);
}
