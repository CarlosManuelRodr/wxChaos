#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class MandelbrotRenderer : public Renderer
{
    int _buddhaRandomP;

    Point TracePoint(double pixelRe, double pixelIm) const;
    void RenderFromPoint(unsigned int (MandelbrotRenderer::*colorPoint)(const Point&) const);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void TriangleInequalityRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;
    unsigned int ColorTriangleInequalityPoint(const Point& point) const;
    void BuddhabrotRender();

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
