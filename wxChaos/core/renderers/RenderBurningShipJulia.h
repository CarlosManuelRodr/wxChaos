#pragma once
#include "../FractalClasses.h"

/*
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class RenderBurningShipJulia : public RenderFractal
{
public:
    RenderBurningShipJulia();
    void Render();
};
