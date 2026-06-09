#pragma once
#include "../Renderer.h"

/*
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class JuliaZNRenderer : public Renderer
{
    int _n;
    double _bailout;

    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (JuliaZNRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;

public:
    JuliaZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
