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
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void TriangleInequalityRender();
    void BuddhabrotRender() const;

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
