#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderMagnet
* @brief Threaded Magnet rendering routines.
*/
class RenderMagnet : public RenderFractal
{
public:
    RenderMagnet();
    void Render();
};
