#pragma once
#include "../Renderer.h"

/**
* @class JellyfishRenderer
* @brief Renders the Jellyfish escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Jellyfish and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class JellyfishRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    JellyfishRenderer();

    void Render() override;
};
