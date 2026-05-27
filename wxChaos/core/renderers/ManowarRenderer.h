#pragma once
#include "../Renderer.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class ManowarRenderer : public Renderer
{
public:
    ManowarRenderer();
    void Render() override;
    void SpecialRender() override;
};
