#pragma once
#include "../RenderFractal.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class RenderManowarJulia : public RenderFractal
{
public:
    RenderManowarJulia();

    void Render() override;
    void SpecialRender() override;
};
