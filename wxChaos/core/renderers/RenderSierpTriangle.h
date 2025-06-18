#pragma once
#include "../FractalClasses.h"

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
