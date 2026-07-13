#pragma once
#include "../RasterFractal.h"
#include "../renderers/BurningShipJuliaRenderer.h"

/**
 * @class BurningShipJulia
 * @brief Julia variant of the Burning Ship recurrence.
 *
 * Each pixel supplies z_0 and the UI Julia constant supplies k. The orbit
 * iterates z_{n+1} = (abs(Re(z_n)) + i abs(Im(z_n)))^2 + k. Points escape
 * when |z|^2 exceeds 4.
 */
class BurningShipJulia : public RasterFractal
{
private:
    BurningShipJuliaRenderer *myRender;
public:
    BurningShipJulia(unsigned int width, unsigned int height);
    ~BurningShipJulia();
    wxString GetName() const override { return "Burning Ship Julia"; }

    void Render();
    void DrawOrbit();
};
