#pragma once
#include "../Fractal.h"
#include "../renderers/SinoidalRenderer.h"

/**
 * @class Sinoidal
 * @brief Sine-map Julia-style escape-time fractal.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = k sin(z_n). This renderer treats the point as escaped
 * when |z|^2 exceeds max iterations.
 */
class Sinoidal : public Fractal
{
    SinoidalRenderer* myRender;
public:
    Sinoidal(unsigned int width, unsigned int height);
    ~Sinoidal() override;
    wxString GetName() const override { return wxT("Sine"); }

    void Render() override;
    void DrawOrbit() override;
};
