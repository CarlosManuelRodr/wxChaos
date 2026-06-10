#pragma once
#include "../Renderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    JuliaRenderer();
    void Render() override;
};
