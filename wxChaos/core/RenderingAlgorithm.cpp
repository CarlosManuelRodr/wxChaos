#include "RenderingAlgorithm.h"

RenderingAlgorithmOptions RenderingAlgorithm::GetDefaultOptions(const RenderingAlgorithmType algorithm)
{
    RenderingAlgorithmOptions options;

    switch (algorithm)
    {
        case RenderingAlgorithmType::GaussianInt:
            options.relativeColor = true;
            break;
        case RenderingAlgorithmType::Buddhabrot:
            options.relativeColor = true;
            options.paletteSize = 1500;
            break;
        default:
            break;
    }

    return options;
}

bool RenderingAlgorithm::UsesRelativeColorByDefault(const RenderingAlgorithmType algorithm)
{
    return GetDefaultOptions(algorithm).relativeColor;
}

int RenderingAlgorithm::GetDefaultPaletteSize(const RenderingAlgorithmType algorithm, const int fallbackPaletteSize)
{
    const RenderingAlgorithmOptions options = GetDefaultOptions(algorithm);
    return options.paletteSize.value_or(fallbackPaletteSize);
}
