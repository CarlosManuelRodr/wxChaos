#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderSierpTriangle
* @brief Threaded SierpTriangle rendering routines.
*/
class RenderSierpTriangle : public RenderFractal
{
public:
    RenderSierpTriangle();
    void Render();
};
