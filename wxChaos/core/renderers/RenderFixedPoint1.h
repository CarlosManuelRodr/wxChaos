#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint1
* @brief Threaded FixedPoint1 rendering routines.
*/
class RenderFixedPoint1 : public RenderFractal
{
    double _minStep;
public:
    RenderFixedPoint1();

    void Render() override;
    void SetParams(double minStep);
};
