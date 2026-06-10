#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class MandelbrotRenderer : public Renderer
{
    int _buddhaRandomP;

    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    void BuddhabrotRender();

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
