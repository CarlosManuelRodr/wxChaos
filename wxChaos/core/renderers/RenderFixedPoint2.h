#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint2
* @brief Threaded FixedPoint2 rendering routines.
*/
class RenderFixedPoint2 : public RenderFractal
{
    double _minStep;
public:
    RenderFixedPoint2();

    void Render() override;
    void SetParams(double minStep);
};
