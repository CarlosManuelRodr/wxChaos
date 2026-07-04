#pragma once
#include "../RenderWorker.h"

/**
* @class MandelbrotZMRenderer
* @brief Renders Multibrot samples for one pixel region.
*
* The renderer owns the formula trace for MandelbrotZN and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class MandelbrotZMRenderer : public RenderWorker
{
    int _n;
    double _bailout;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    MandelbrotZMRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
