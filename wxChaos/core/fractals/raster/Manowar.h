#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/raster/ManowarRenderer.h"

/**
 * @class Manowar
 * @brief Manowar escape-time fractal with one-step orbit memory.
 *
 * Each pixel is the complex parameter c. The orbit starts with z_0 = c and
 * m_0 = c, then iterates z_{n+1} = z_n^2 + m_n + c and m_{n+1} = z_n. Points
 * escape when |z|^2 exceeds 4.
 */
class Manowar : public RasterFractal
{
    ManowarRenderer* myRender;
public:
    Manowar(unsigned int width, unsigned int height);
    ~Manowar() override;
    wxString GetName() const override { return "Manowar"; }

    void Render() override;
    void DrawOrbit() override;
};
