#pragma once
#include "../RenderFractal.h"

/*
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class RenderNewton : public RenderFractal
{
    double minStep;
public:
    RenderNewton();
    void Render() override;
    void SpecialRender() override;
    void SetParams(double _minStep);
};
