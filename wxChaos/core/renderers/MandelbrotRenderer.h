#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class MandelbrotRenderer : public Renderer
{
    int _buddhaRandomP;

    void EscapeTimeRender();
    EscapePoint IterateEscapePoint(double pixelRe, double pixelIm) const;
    unsigned int ColorEscapePoint(const EscapePoint& point) const;
    void GaussianIntRender();
    GaussianIntegerPoint IterateGaussianIntegerPoint(double pixelRe, double pixelIm) const;
    void EscapeAngleRender();
    EscapePoint IterateEscapeAnglePoint(double pixelRe, double pixelIm) const;
    void TriangleInequalityRender();
    TriangleInequalityPoint IterateTriangleInequalityPoint(double pixelRe, double pixelIm) const;
    void BuddhabrotRender();

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
