#pragma once
#include "../Renderer.h"

/*
* @class RenderTricorn
* @brief Threaded Tricorn rendering routines.
*/
class TricornRenderer : public Renderer
{
public:
    TricornRenderer();
    void Render() override;
};
