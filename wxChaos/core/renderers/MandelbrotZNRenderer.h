#pragma once
#include "../RenderWorker.h"

/**
* @class MandelbrotZNRenderer
* @brief Renders Multibrot samples for one pixel region.
*
* The renderer owns the formula trace for MandelbrotZN and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class MandelbrotZNRenderer : public RenderWorker
{
    int _n;
    double _bailout;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
