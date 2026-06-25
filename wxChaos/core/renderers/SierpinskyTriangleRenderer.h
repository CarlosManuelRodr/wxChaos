#pragma once
#include "../Renderer.h"

/**
* @class SierpinskyTriangleRenderer
* @brief Renders the Sierpinski triangle inverse-map recurrence.
*
* The renderer iterates each pixel through the triangle mapping and writes
* escape/convergence values into the shared output maps.
*/
class SierpinskyTriangleRenderer : public Renderer
{
public:
    SierpinskyTriangleRenderer();
    void Render() override;
};
