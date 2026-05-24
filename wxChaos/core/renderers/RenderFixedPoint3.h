#pragma once
#include "../RenderFractal.h"

/*
* @class RenderFixedPoint3
* @brief Threaded FixedPoint3 rendering routines.
*/
class RenderFixedPoint3 : public RenderFractal
{
    double _minStep;
public:
    RenderFixedPoint3();

    void Render() override;
    void SetParams(double minStep);
};
