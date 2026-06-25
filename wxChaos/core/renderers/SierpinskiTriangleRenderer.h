#pragma once
#include "../RenderWorker.h"

/**
* @class SierpinskiTriangleRenderer
* @brief Renders the Sierpinski triangle inverse-map recurrence.
*
* The renderer iterates each pixel through the triangle mapping and writes
* escape/convergence values into the shared output maps.
*/
class SierpinskiTriangleRenderer : public RenderWorker
{
public:
    SierpinskiTriangleRenderer();
    void Render() override;
};
