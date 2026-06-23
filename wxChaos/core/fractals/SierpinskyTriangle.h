#pragma once
#include "../Fractal.h"
#include "../renderers/SierpinskyTriangleRenderer.h"

/**
 * @class SierpinskyTriangle
 * @brief Escape-time inverse-map rendering of the Sierpinski triangle.
 *
 * Each pixel supplies z_0. While |z|^2 <= 4, the renderer applies one of three
 * affine branches: z <- 2z - i when Im(z) > 0.5, z <- 2z - 1 when Re(z) > 0.5,
 * otherwise z <- 2z. Points that never escape within max iterations are marked
 * as inside.
 */
class SierpinskyTriangle : public Fractal
{
    SierpinskyTriangleRenderer* myRender;
public:
    SierpinskyTriangle(unsigned int width, unsigned int height);
    wxString GetName() const override { return wxT("Sierpinski Triangle"); }

    void Render() override;
};
