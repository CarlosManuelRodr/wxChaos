#pragma once
#include "../RenderFractal.h"

/*
* @class RenderSierpinskyTriangle
* @brief Threaded SierpinskyTriangle rendering routines.
*/
class RenderSierpinskyTriangle : public RenderFractal
{
public:
    RenderSierpinskyTriangle();
    void Render() override;
};
