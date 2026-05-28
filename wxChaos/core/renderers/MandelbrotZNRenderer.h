#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class MandelbrotZNRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    int _n;
    double _bailout;
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();
public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
