#pragma once
#include "../Renderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public Renderer
{
public:
    JuliaRenderer();
    void Render() override;
    void SpecialRender() override;
};
