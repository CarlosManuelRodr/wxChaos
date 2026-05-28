#pragma once
#include "../Renderer.h"

/*
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class NewtonRenderer : public Renderer
{
    double minStep;

    void ConvergenceTestRender();
    void ConvergenceTestWithOrbitTrapRender();

public:
    NewtonRenderer();
    void Render() override;
    void SetParams(double _minStep);
};
