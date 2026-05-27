#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class BurningShipJuliaRenderer : public Renderer
{
public:
    BurningShipJuliaRenderer();
    void Render() override;
};
