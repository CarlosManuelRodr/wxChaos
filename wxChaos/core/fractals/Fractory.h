#pragma once
#include "../Fractal.h"
#include "../renderers/FractoryRenderer.h"

/**
 * @class Fractory
 * @brief Escape-time map using a coupled z and b recurrence.
 *
 * Each pixel is the complex parameter c. The orbit starts with z_0 = c and
 * b_0 = c - sin(c), then iterates b_{n+1} = c + b_n / c - z_n and
 * z_{n+1} = z_n c + b_{n+1} / z_n. Points escape when |z|^2 exceeds 4.
 */
class Fractory : public Fractal
{
    FractoryRenderer* myRender;
public:
    Fractory(unsigned int width, unsigned int height);
    ~Fractory() override;
    wxString GetName() const override { return "Fractory"; }

    void Render() override;
    void DrawOrbit() override;
};
