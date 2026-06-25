#pragma once
#include "../RenderWorker.h"

/**
* @class SinoidalRenderer
* @brief Renders the Sinoidal escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Sinoidal and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class SinoidalRenderer : public RenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    SinoidalRenderer();

    void Render() override;
};
