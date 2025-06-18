#pragma once
#include "../RenderFractal.h"

/*
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class RenderJuliaZN : public RenderFractal
{
private:
    int n;
    double bailout;
public:
    RenderJuliaZN();

    void Render();
    void SpecialRender();
    void SetParams(int _n, double _bailout);
};
