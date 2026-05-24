#pragma once
#include "../RenderFractal.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class RenderMandelbrot : public RenderFractal
{
    int _buddhaRandomP;
    int _bd;
public:
    RenderMandelbrot();

    void Render() override;
    void SpecialRender() override;
    int AskProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
