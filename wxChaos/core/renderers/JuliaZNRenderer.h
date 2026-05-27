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
public:
    JuliaZNRenderer();

    void Render() override;
    void SpecialRender() override;
    void SetParams(int n, double bailout);
};
