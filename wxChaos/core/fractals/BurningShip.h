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
    BurningShip(int width, int height);
    ~BurningShip() override;

    void Render() override;
    void DrawOrbit() override;
};
