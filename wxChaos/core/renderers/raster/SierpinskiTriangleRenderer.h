#pragma once
#include "../../raster/RasterRenderWorker.h"

/**
* @class SierpinskiTriangleRenderer
* @brief Renders the Sierpinski triangle inverse-map recurrence.
*
* The renderer iterates each pixel through the triangle mapping and writes
* escape/convergence values into the shared output maps.
*/
class SierpinskiTriangleRenderer : public RasterRenderWorker
{
public:
    SierpinskiTriangleRenderer();
    void Render() override;
};
