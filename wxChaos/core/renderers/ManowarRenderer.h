#pragma once
#include "../raster/RasterRenderWorker.h"

/**
* @class ManowarRenderer
* @brief Renders the Manowar escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Manowar and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class ManowarRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    ManowarRenderer();
    void Render() override;
};
