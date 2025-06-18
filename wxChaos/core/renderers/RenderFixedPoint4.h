#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint4
* @brief Threaded FixedPoint4 rendering routines.
*/
class RenderFixedPoint4 : public RenderFractal
{
private:
    double minStep;
public:
    RenderFixedPoint4();

    void Render();
    void SetParams(double _minStep);
};
