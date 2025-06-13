#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class RenderFractory : public RenderFractal
{
public:
    RenderFractory();
    void Render();
};
