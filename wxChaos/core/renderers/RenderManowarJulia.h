#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class RenderManowarJulia : public RenderFractal
{
public:
    RenderManowarJulia();

    void Render();
    void SpecialRender();
};
