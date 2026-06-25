#pragma once
#include "../Renderer.h"

/**
* @class MagnetRenderer
* @brief Renders Magnet-type escape-time samples for one pixel region.
*
* The renderer owns the formula trace for Magnet and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class MagnetRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    MagnetRenderer();
    void Render() override;
};
