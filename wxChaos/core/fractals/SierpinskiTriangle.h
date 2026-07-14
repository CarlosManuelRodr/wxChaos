#pragma once
#include "../raster/RasterFractal.h"
#include "../renderers/SierpinskiTriangleRenderer.h"

/**
 * @class SierpinskiTriangle
 * @brief Escape-time inverse-map rendering of the Sierpinski triangle.
 *
 * Each pixel supplies z_0. While |z|^2 <= 4, the renderer applies one of three
 * affine branches: z <- 2z - i when Im(z) > 0.5, z <- 2z - 1 when Re(z) > 0.5,
 * otherwise z <- 2z. Points that never escape within max iterations are marked
 * as inside.
 */
class SierpinskiTriangle : public RasterFractal
{
    SierpinskiTriangleRenderer* myRender;
public:
    SierpinskiTriangle(unsigned int width, unsigned int height);
    wxString GetName() const override { return "Sierpinski Triangle"; }
    CoordinateSystem GetCoordinateSystem() const override { return {_("x"), _("y")}; }

    void Render() override;
};
