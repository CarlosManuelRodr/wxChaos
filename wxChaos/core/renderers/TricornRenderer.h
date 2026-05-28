#pragma once
#include "../Renderer.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class TricornRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
public:
    TricornRenderer();
    void Render() override;
};
