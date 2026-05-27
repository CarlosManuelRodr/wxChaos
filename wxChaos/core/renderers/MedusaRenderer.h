#pragma once
#include "../Renderer.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class MedusaRenderer : public Renderer
{
public:
    MedusaRenderer();

    void Render() override;
    void SpecialRender() override;
};
