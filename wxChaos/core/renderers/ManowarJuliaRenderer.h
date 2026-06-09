#pragma once
#include "../Renderer.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class ManowarJuliaRenderer : public Renderer
{
    Point TracePoint(double pixelRe, double pixelIm) const;
    void RenderFromPoint(unsigned int (ManowarJuliaRenderer::*colorPoint)(const Point&) const);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;

public:
    ManowarJuliaRenderer();

    void Render() override;
};
