#pragma once
#include "../../raster/RasterRenderWorker.h"

/**
* @class FixedPoint3Renderer
* @brief Renders the tan(z) fixed-point convergence map for one pixel region.
*
* The renderer iterates each pixel as the initial complex value and colors the
* result using the shared convergence-test output maps.
*/
class FixedPoint3Renderer : public RasterRenderWorker
{
    double _minStep;

    template<class Real>
    void RenderConvergence();

public:
    FixedPoint3Renderer();

    void Render() override;
    void SetParams(double minStep);
};
