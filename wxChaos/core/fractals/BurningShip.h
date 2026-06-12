#pragma once
#include "../Fractal.h"
#include "../renderers/BurningShipRenderer.h"

/*
* @class BurningShip
* @brief Handles the RenderBurningShip threads.
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
