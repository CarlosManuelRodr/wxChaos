#pragma once
#include "../Fractal.h"
#include "../renderers/SinusoidalRenderer.h"

/**
 * @class Sinusoidal
 * @brief Sine-map Julia-style escape-time fractal.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = k sin(z_n). This renderer treats the point as escaped
 * when |z|^2 exceeds max iterations.
 */
class Sinusoidal : public Fractal
{
    SinusoidalRenderer* myRender;
public:
    Sinusoidal(unsigned int width, unsigned int height);
    ~Sinusoidal() override;
    wxString GetName() const override { return "Sine"; }

    void Render() override;
    void DrawOrbit() override;
};
