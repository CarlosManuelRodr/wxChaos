#pragma once
#include "../FractalClasses.h"

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
