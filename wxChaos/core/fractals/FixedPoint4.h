#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint4Renderer.h"

/**
 * @class FixedPoint4
 * @brief Fixed-point convergence map for z_{n+1} = z_n^2.
 *
 * Each pixel supplies z_0. Iteration stops when the real and imaginary parts
 * both change by less than the configured minimum step, or when max iterations
 * is reached. Coloring is based on the convergence iteration and final real
 * half-plane.
 */
class FixedPoint4 : public Fractal
{
    FixedPoint4Renderer* myRender;
    double minStep;
public:
    FixedPoint4(unsigned int width, unsigned int height);
    ~FixedPoint4() override;
    wxString GetName() const override { return "Fixed Point: z^2"; }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
