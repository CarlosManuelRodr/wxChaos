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

    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();

public:
    JuliaZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
