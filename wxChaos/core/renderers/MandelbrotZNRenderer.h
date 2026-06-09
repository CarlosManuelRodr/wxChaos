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

    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
