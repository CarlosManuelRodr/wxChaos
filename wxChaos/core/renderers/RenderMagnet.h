#pragma once
#include "../RenderFractal.h"

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
