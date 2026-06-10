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
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (JuliaRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void TriangleInequalityRender();

public:
    JuliaRenderer();
    void Render() override;
};
