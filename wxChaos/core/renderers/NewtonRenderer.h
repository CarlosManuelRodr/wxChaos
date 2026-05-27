#pragma once
#include "../Renderer.h"

/*
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class NewtonRenderer : public Renderer
{
    double minStep;
public:
    NewtonRenderer();
    void Render() override;
    void SpecialRender() override;
    void SetParams(double _minStep);
};
