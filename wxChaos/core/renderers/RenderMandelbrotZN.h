#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class RenderMandelbrotZN : public RenderFractal
{
private:
    int n;
    double bailout;
public:
    RenderMandelbrotZN();

    void Render();
    void SpecialRender();
    void SetParams(int _n, double _bailout);
};
