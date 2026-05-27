#pragma once
#include "../Renderer.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class ManowarJuliaRenderer : public Renderer
{
public:
    ManowarJuliaRenderer();

    void Render() override;
    void SpecialRender() override;
};
