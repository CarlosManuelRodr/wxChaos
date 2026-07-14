#pragma once
#include "../raster/RasterRenderWorker.h"

/**
* @class FixedPoint4Renderer
* @brief Renders the z^2 fixed-point convergence map for one pixel region.
*
* The renderer iterates each pixel as the initial complex value and colors the
* result using the shared convergence-test output maps.
*/
class FixedPoint4Renderer : public RasterRenderWorker
{
    double _minStep;

    template<class Real>
    void RenderConvergence();

public:
    FixedPoint4Renderer();

    void Render() override;
    void SetParams(double minStep);
};
