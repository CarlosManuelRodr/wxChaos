#pragma once
#include "../Renderer.h"

/*
* @class RenderMagnet
* @brief Threaded Magnet rendering routines.
*/
class MagnetRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    MagnetRenderer();
    void Render() override;
};
