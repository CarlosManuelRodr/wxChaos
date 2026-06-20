#pragma once
#include "../Renderer.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class TricornRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    TricornRenderer();
    void Render() override;
};
