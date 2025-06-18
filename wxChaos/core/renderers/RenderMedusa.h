#pragma once
#include "../FractalClasses.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class RenderMedusa : public RenderFractal
{
public:
    RenderMedusa();

    void Render();
    void SpecialRender();
};
