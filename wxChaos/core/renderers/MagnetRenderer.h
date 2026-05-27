#pragma once
#include "../Renderer.h"

/*
* @class RenderMagnet
* @brief Threaded Magnet rendering routines.
*/
class MagnetRenderer : public Renderer
{
public:
    MagnetRenderer();
    void Render() override;
};
