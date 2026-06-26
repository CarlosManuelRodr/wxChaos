#pragma once
#include "../Fractal.h"
#include "../renderers/MagnetRenderer.h"

/**
 * @class Magnet
 * @brief Magnet-type escape-time fractal.
 *
 * Each pixel is the complex parameter c. The orbit starts at z_0 = 0 and
 * iterates z_{n+1} = ((z_n^2 + c - 1) / (2 z_n + c - 2))^2. This renderer
 * treats the point as escaped when |z|^2 exceeds max iterations.
 */

class Magnet : public Fractal
{
    MagnetRenderer* myRender;
public:
    Magnet(unsigned int width, unsigned int height);
    ~Magnet() override;
    wxString GetName() const override { return "Magnet"; }

    void Render() override;
    void DrawOrbit() override;
};
