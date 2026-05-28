#pragma once
#include "../Renderer.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class MedusaRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void EscapeTimeWithOrbitTrapRender();
public:
    MedusaRenderer();

    void Render() override;
};
