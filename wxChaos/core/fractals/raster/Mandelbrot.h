#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/MandelbrotRenderer.h"

/**
 * @class Mandelbrot
 * @brief Quadratic Mandelbrot set and Buddhabrot sampler.
 *
 * Each pixel is the complex parameter c. The standard orbit starts at z_0 = 0
 * and iterates z_{n+1} = z_n^2 + c, with escape at |z|^2 > 4. Deep zooms can
 * use perturbation rendering for the same recurrence. Buddhabrot mode randomly
 * samples c values and accumulates the escaped orbits instead of coloring each
 * pixel independently.
 */
class Mandelbrot : public RasterFractal
{
    MandelbrotRenderer* myRender;
    int buddhaRandomP;
public:
    Mandelbrot(unsigned int width, unsigned int height);
    ~Mandelbrot() override;
    wxString GetName() const override { return "Mandelbrot"; }

    void Render() override;     ///< Launch threads.
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
    void PreRender() override;
    void PreDrawMaps() override;
};
