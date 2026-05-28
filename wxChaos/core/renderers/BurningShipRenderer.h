#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class BurningShipRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();

public:
    BurningShipRenderer();
    void Render() override;
};
