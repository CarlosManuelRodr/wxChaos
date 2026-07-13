#pragma once
#include "../RasterFractal.h"
#include "../renderers/FixedPoint2Renderer.h"

/**
 * @class FixedPoint2
 * @brief Fixed-point convergence map for z_{n+1} = cos(z_n).
 *
 * Each pixel supplies z_0. Iteration stops when the real and imaginary parts
 * both change by less than the configured minimum step, or when max iterations
 * is reached. Coloring is based on the convergence iteration and final real
 * half-plane.
 */
class FixedPoint2 : public RasterFractal
{
    FixedPoint2Renderer* myRender;
    double minStep;
public:
    FixedPoint2(unsigned int width, unsigned int height);
    ~FixedPoint2() override;
    wxString GetName() const override { return "Fixed Point: cos(z)"; }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
};
