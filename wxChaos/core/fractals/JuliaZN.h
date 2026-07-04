#pragma once
#include "../Fractal.h"
#include "../renderers/JuliaZNRenderer.h"

/**
 * @class JuliaZN
 * @brief Power Julia set with configurable exponent and bailout.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = z_n^p + k, where p is the panel exponent. Escape is
 * detected when |z|^2 exceeds bailout^2.
 */
class JuliaZN: public Fractal
{
    JuliaZNRenderer* myRender;
    int n;
    double bailout;
public:
    JuliaZN(unsigned int width, unsigned int height);
    ~JuliaZN() override;
    wxString GetName() const override { return "Julia ZN"; }

    void CopyOptionFromPanel() override;
    void Render() override;
    void DrawOrbit() override;
};
