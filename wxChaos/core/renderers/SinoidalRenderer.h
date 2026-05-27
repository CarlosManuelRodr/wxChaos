#pragma once
#include "../Renderer.h"

/*
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class SinoidalRenderer : public Renderer
{
public:
    SinoidalRenderer();

    void Render() override;
    void SpecialRender() override;
};
