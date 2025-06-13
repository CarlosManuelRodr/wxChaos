#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderFixedPoint1
* @brief Threaded FixedPoint1 rendering routines.
*/
class RenderFixedPoint1 : public RenderFractal
{
private:
    double minStep;
public:
    RenderFixedPoint1();

    void Render();
    void SetParams(double _minStep);
};
