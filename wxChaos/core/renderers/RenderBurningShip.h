#pragma once
#include "../FractalClasses.h"

/*
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class RenderBurningShip : public RenderFractal
{
public:
    RenderBurningShip();
    void Render();
};
