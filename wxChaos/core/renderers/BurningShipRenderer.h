#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class BurningShipRenderer : public Renderer
{
public:
    BurningShipRenderer();
    void Render() override;
};
