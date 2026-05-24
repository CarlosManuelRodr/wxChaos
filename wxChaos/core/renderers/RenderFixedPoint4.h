#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint4
* @brief Threaded FixedPoint4 rendering routines.
*/
class RenderFixedPoint4 : public RenderFractal
{
    double _minStep;
public:
    RenderFixedPoint4();

    void Render() override;
    void SetParams(double minStep);
};
