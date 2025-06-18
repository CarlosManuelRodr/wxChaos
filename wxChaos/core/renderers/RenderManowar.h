#pragma once
#include "../RenderFractal.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class RenderManowar : public RenderFractal
{
public:
    RenderManowar();
    void Render();
    void SpecialRender();
};
