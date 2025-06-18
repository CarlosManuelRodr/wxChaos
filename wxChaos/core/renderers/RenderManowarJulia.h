#pragma once
#include "../FractalClasses.h"

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
