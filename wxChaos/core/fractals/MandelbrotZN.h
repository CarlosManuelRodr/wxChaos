#pragma once
#include "../Fractal.h"
#include "../renderers/MandelbrotZNRenderer.h"

/**
 * @class MandelbrotZN
 * @brief Multibrot set with configurable exponent and bailout.
 *
 * Each pixel is the complex parameter c. The orbit starts at z_0 = 0 and
 * iterates z_{n+1} = z_n^m + c, where m is the panel exponent. Escape is
 * detected when |z|^2 exceeds bailout^2.
 */
class MandelbrotZN : public Fractal
{
private:
    int n;
    double bailout;
    MandelbrotZNRenderer* myRender;
public:
    MandelbrotZN(unsigned int width, unsigned int height);
    ~MandelbrotZN() override;
    wxString GetName() const override { return "Mandelbrot Z^m"; }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
};
