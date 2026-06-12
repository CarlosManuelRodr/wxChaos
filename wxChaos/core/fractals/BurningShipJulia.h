#pragma once
#include "../Fractal.h"
#include "../renderers/BurningShipJuliaRenderer.h"

/*
* @class BurningShipJulia
* @brief Handles the RenderBurningShipJulia threads.
*/
class BurningShipJulia : public Fractal
{
private:
    BurningShipJuliaRenderer *myRender;
public:
    BurningShipJulia(unsigned int width, unsigned int height);
    ~BurningShipJulia();
    wxString GetName() const override { return wxT("Burning Ship Julia"); }

    void Render();
    void DrawOrbit();
};
