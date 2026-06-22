#pragma once

#include "types/RenderingAlgorithmType.h"

struct RenderingAlgorithmOptions
{
    bool relativeColor = false;
};

class RenderingAlgorithm
{
public:
    static RenderingAlgorithmOptions GetDefaultOptions(RenderingAlgorithmType algorithm);
    static bool UsesRelativeColorByDefault(RenderingAlgorithmType algorithm);
};
