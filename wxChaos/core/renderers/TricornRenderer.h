#pragma once
#include "../Renderer.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class TricornRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    TricornRenderer();
    void Render() override;
};
