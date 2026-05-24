#pragma once
#include "../RenderFractal.h"

/*
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class RenderSinoidal : public RenderFractal
{
public:
    RenderSinoidal();

    void Render() override;
    void SpecialRender() override;
};
