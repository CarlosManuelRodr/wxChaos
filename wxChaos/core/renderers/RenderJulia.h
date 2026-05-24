#pragma once
#include "../RenderFractal.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class RenderJulia : public RenderFractal
{
public:
    RenderJulia();
    void Render() override;
    void SpecialRender() override;
};
