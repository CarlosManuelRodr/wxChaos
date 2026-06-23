#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint3Renderer.h"

/**
 * @class FixedPoint3
 * @brief Fixed-point convergence map for z_{n+1} = tan(z_n).
 *
 * Each pixel supplies z_0. Iteration stops when the real and imaginary parts
 * both change by less than the configured minimum step, or when max iterations
 * is reached. Coloring is based on the convergence iteration and final real
 * half-plane.
 */
class FixedPoint3 : public Fractal
{
    FixedPoint3Renderer* myRender;
    double minStep;
public:
    FixedPoint3(unsigned int width, unsigned int height);
    ~FixedPoint3() override;
    wxString GetName() const override { return wxT("Fixed Point: tan(z)"); }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
