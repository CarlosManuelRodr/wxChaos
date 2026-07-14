#pragma once
#include "../../raster/RasterRenderWorker.h"

/**
* @class TricornRenderer
* @brief Renders Tricorn escape-time samples for one pixel region.
*
* The renderer owns the formula trace for Tricorn and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class TricornRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    TricornRenderer();
    void Render() override;
};
