#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/raster/FixedPoint1Renderer.h"

/**
 * @class FixedPoint1
 * @brief Fixed-point convergence map for z_{n+1} = sin(z_n).
 *
 * Each pixel supplies z_0. Iteration stops when the real and imaginary parts
 * both change by less than the configured minimum step, or when max iterations
 * is reached. Coloring is based on the convergence iteration and final real
 * half-plane.
 */
class FixedPoint1 : public RasterFractal
{
    FixedPoint1Renderer* myRender;
    double minStep;
public:
    FixedPoint1(unsigned int width, unsigned int height);
    ~FixedPoint1() override;
    wxString GetName() const override { return "Fixed Point: sin(z)"; }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
};
