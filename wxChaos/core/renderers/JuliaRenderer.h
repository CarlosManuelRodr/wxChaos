#pragma once
#include "../Renderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public Renderer
{
    Point TracePoint(double pixelRe, double pixelIm) const;
    void RenderFromPoint(unsigned int (JuliaRenderer::*colorPoint)(const Point&) const);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void TriangleInequalityRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;
    unsigned int ColorTriangleInequalityPoint(const Point& point) const;

public:
    JuliaRenderer();
    void Render() override;
};
