#pragma once
#include "../RenderWorker.h"

/**
* @class NewtonRenderer
* @brief Renders Newton-Raphson basins for z^3 - 1.
*
* The renderer iterates each pixel as the initial value, assigns converged
* points to the built-in roots, and records convergence data in the shared
* output maps.
*/
class NewtonRenderer : public RenderWorker
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
