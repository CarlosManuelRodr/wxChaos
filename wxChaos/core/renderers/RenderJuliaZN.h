#pragma once
#include "../RenderFractal.h"

/*
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class RenderJuliaZN : public RenderFractal
{
    int _n;
    double _bailout;
public:
    RenderJuliaZN();

    void Render() override;
    void SpecialRender() override;
    void SetParams(int n, double bailout);
};
