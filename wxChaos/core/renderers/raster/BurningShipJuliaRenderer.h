#pragma once
#include "../../raster/RasterRenderWorker.h"

/**
* @class BurningShipJuliaRenderer
* @brief Renders the Burning Ship Julia recurrence for one pixel region.
*
* The renderer owns the formula trace for BurningShipJulia and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class BurningShipJuliaRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    BurningShipJuliaRenderer();
    void Render() override;
};
