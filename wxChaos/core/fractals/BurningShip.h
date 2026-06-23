#pragma once
#include "../Fractal.h"
#include "../renderers/BurningShipRenderer.h"

/**
 * @class BurningShip
 * @brief Escape-time Burning Ship set.
 *
 * Each pixel is the complex parameter c. The orbit starts at z_0 = 0 and
 * iterates z_{n+1} = (abs(Re(z_n)) + i abs(Im(z_n)))^2 + c. Points escape
 * when |z|^2 exceeds 4.
 */
class BurningShip : public Fractal
{
    BurningShipRenderer* myRender;
public:
    BurningShip(unsigned int width, unsigned int height);
    ~BurningShip() override;
    wxString GetName() const override { return wxT("Burning Ship"); }

    void Render() override;
    void DrawOrbit() override;
};
