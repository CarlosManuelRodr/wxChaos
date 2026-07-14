#pragma once
#include "../raster/RasterFractal.h"
#include "../renderers/JuliaRenderer.h"

/**
 * @class Julia
 * @brief Quadratic Julia set.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = z_n^2 + k. Points escape when |z|^2 exceeds 4.
 */
class Julia: public RasterFractal
{
    JuliaRenderer* myRender;
public:
    Julia(unsigned int width, unsigned int height);
    ~Julia();
    wxString GetName() const override { return "Julia"; }

    void Render();
    void DrawOrbit();
};
