#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class BurningShipJuliaRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
public:
    BurningShipJuliaRenderer();
    void Render() override;
};
