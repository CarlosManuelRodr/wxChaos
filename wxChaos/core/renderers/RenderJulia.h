#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class RenderJulia : public RenderFractal
{
public:
    RenderJulia();
    void Render();
    void SpecialRender();
};
