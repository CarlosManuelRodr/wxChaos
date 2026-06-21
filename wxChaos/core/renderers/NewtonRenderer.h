#pragma once
#include "../Renderer.h"

/*
* @class RenderNewton
* @brief Threaded Newton rendering routines.
*/
class NewtonRenderer : public Renderer
{
    double minStep;

    template<class Real>
    void RenderConvergenceTest();
    template<class Real>
    void RenderConvergenceTestWithOrbitTrap();
    void ConvergenceTestRender();
    void ConvergenceTestWithOrbitTrapRender();

public:
    NewtonRenderer();
    void Render() override;
    void SetParams(double _minStep);
};
