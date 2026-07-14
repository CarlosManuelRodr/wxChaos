#pragma once
#include "../raster/RasterRenderWorker.h"

/**
* @class BurningShipRenderer
* @brief Renders the Burning Ship set for one pixel region.
*
* The renderer owns the formula trace for BurningShip and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class BurningShipRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    BurningShipRenderer();
    void Render() override;
};
