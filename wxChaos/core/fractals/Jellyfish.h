#pragma once
#include "../RasterFractal.h"
#include "../renderers/JellyfishRenderer.h"

/**
 * @class Jellyfish
 * @brief Julia-style escape-time fractal using a fractional power.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = z_n^1.5 + k using the complex power implementation.
 * Points escape when |z|^2 exceeds 4.
 */
class Jellyfish : public RasterFractal
{
    JellyfishRenderer* myRender;
public:
    Jellyfish(unsigned int width, unsigned int height);
    ~Jellyfish() override;
    wxString GetName() const override { return "Jellyfish"; }

    void Render() override;
    void DrawOrbit() override;
};
