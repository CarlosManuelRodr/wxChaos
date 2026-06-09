#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class MandelbrotZNRenderer : public Renderer
{
    int _n;
    double _bailout;

    Point TracePoint(double pixelRe, double pixelIm) const;
    void RenderFromPoint(unsigned int (MandelbrotZNRenderer::*colorPoint)(const Point&) const);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
