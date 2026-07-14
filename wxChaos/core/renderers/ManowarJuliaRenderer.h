#pragma once
#include "../RasterRenderWorker.h"

/**
* @class ManowarJuliaRenderer
* @brief Renders the Manowar Julia recurrence for one pixel region.
*
* The renderer owns the formula trace for ManowarJulia and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class ManowarJuliaRenderer : public RasterRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    ManowarJuliaRenderer();

    void Render() override;
};
