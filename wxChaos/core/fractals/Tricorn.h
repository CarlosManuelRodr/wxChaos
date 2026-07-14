#pragma once
#include "../raster/RasterFractal.h"
#include "../renderers/TricornRenderer.h"

/**
 * @class Tricorn
 * @brief Antiholomorphic quadratic Mandelbrot variant.
 *
 * Each pixel is the complex parameter c. The orbit starts at z_0 = 0 and
 * iterates z_{n+1} = conjugate(z_n)^2 + c. Points escape when |z|^2 exceeds 4.
 */
class Tricorn : public RasterFractal
{
private:
    TricornRenderer* myRender;
public:
    Tricorn(unsigned int width, unsigned int height);
    ~Tricorn();
    wxString GetName() const override { return "Tricorn"; }

    void Render();
    void DrawOrbit();
};
