#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint3
* @brief Threaded FixedPoint3 rendering routines.
*/
class RenderFixedPoint3 : public RenderFractal
{
private:
    double minStep;
public:
    RenderFixedPoint3();

    void Render();
    void SetParams(double _minStep);
};
