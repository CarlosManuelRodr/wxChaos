#pragma once
#include "../RenderWorker.h"

/**
* @class FixedPoint1Renderer
* @brief Renders the sin(z) fixed-point convergence map for one pixel region.
*
* The renderer iterates each pixel as the initial complex value and colors the
* result using the shared convergence-test output maps.
*/
class FixedPoint1Renderer : public RenderWorker
{
    double _minStep;

    template<class Real>
    void RenderConvergence();

public:
    FixedPoint1Renderer();

    void Render() override;
    void SetParams(double minStep);
};
