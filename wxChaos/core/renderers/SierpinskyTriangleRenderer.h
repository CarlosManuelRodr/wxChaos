#pragma once
#include "../Renderer.h"

/*
* @class RenderSierpinskyTriangle
* @brief Threaded SierpinskyTriangle rendering routines.
*/
class SierpinskyTriangleRenderer : public Renderer
{
public:
    SierpinskyTriangleRenderer();
    void Render() override;
};
