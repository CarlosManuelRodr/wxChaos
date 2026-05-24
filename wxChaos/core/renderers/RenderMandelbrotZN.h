#pragma once
#include "../RenderFractal.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class RenderMandelbrotZN : public RenderFractal
{
    int _n;
    double _bailout;
public:
    RenderMandelbrotZN();

    void Render() override;
    void SpecialRender() override;
    void SetParams(int n, double bailout);
};
