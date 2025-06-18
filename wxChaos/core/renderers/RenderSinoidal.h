#pragma once
#include "../FractalClasses.h"

/*
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class RenderSinoidal : public RenderFractal
{
public:
    RenderSinoidal();

    void Render();
    void SpecialRender();
};
