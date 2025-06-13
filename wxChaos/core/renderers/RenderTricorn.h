#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class RenderTricorn : public RenderFractal
{
public:
    RenderTricorn();
    void Render();
};
