#pragma once
#include "../raster/RasterFractal.h"
#include "../renderers/NewtonRenderer.h"

/**
 * @class Newton
 * @brief Newton-Raphson convergence fractal for z^3 - 1.
 *
 * Each pixel supplies z_0. The normal renderer applies
 * z_{n+1} = z_n - (z_n^3 - 1) / (3 z_n^2) and stops when the step is smaller
 * than the panel minimum step. Orbit-trap mode keeps the historical variant
 * z_{n+1} = z_n - (z_n^3 - 1) / (2 z_n^2). Coloring distinguishes the root
 * basin and convergence speed.
 */
class Newton : public RasterFractal
{
    NewtonRenderer* myRender;
    double minStep;
public:
    Newton(unsigned int width, unsigned int height);
    ~Newton() override;
    wxString GetName() const override { return "Newton"; }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptionFromPanel() override;
};
