#pragma once
#include "../Renderer.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class ManowarRenderer : public Renderer
{
    Point TracePoint(double pixelRe, double pixelIm) const;
    void RenderFromPoint(unsigned int (ManowarRenderer::*colorPoint)(const Point&) const);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;

public:
    ManowarRenderer();
    void Render() override;
};
