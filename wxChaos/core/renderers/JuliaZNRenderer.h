#pragma once
#include "../Renderer.h"

/*
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class JuliaZNRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    int _n;
    double _bailout;
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();
public:
    JuliaZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
