#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

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
