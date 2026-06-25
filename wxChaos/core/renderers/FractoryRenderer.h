#pragma once
#include "../Renderer.h"

/**
* @class FractoryRenderer
* @brief Renders the Fractory escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Fractory and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class FractoryRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    FractoryRenderer();
    void Render() override;
};
