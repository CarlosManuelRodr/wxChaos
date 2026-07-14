#pragma once
#include "../raster/RasterRenderWorker.h"

/**
* @class FixedPoint2Renderer
* @brief Renders the cos(z) fixed-point convergence map for one pixel region.
*
* The renderer iterates each pixel as the initial complex value and colors the
* result using the shared convergence-test output maps.
*/
class FixedPoint2Renderer : public RasterRenderWorker
{
    double _minStep;

    template<class Real>
    void RenderConvergence();

public:
    FixedPoint2Renderer();

    void Render() override;
    void SetParams(double minStep);
};
