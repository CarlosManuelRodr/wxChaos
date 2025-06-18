#pragma once
#include "../RenderFractal.h"

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
