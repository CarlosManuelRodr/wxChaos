#include "RenderingAlgorithm.h"

RenderingAlgorithmOptions RenderingAlgorithm::GetDefaultOptions(const RenderingAlgorithmType algorithm)
{
    RenderingAlgorithmOptions options;

    switch (algorithm)
    {
        case RenderingAlgorithmType::GaussianInt:
        case RenderingAlgorithmType::Buddhabrot:
            options.relativeColor = true;
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
