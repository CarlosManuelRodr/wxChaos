#pragma once

#include <optional>
#include "types/RenderingAlgorithmType.h"

struct RenderingAlgorithmOptions
{
    bool relativeColor = false;
    std::optional<int> paletteSize;
};

class RenderingAlgorithm
{
public:
    static RenderingAlgorithmOptions GetDefaultOptions(RenderingAlgorithmType algorithm);
    static bool UsesRelativeColorByDefault(RenderingAlgorithmType algorithm);
    static int GetDefaultPaletteSize(RenderingAlgorithmType algorithm, int fallbackPaletteSize);
};
