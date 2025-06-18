#pragma once
#include "../RenderFractal.h"

/*
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class RenderNewton : public RenderFractal
{
private:
    double minStep;
public:
    RenderNewton();

    void Render();
    void SpecialRender();
    void SetParams(double _minStep);
};
