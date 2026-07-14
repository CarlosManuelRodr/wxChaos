#pragma once
#include "../../raster/RasterRenderWorker.h"

/**
* @class SinusoidalRenderer
* @brief Renders the Sinusoidal escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Sinusoidal and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class SinusoidalRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    SinusoidalRenderer();

    void Render() override;
};
