#pragma once
#include "../Renderer.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class ManowarJuliaRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void EscapeTimeWithOrbitTrapRender();

public:
    ManowarJuliaRenderer();

    void Render() override;
};
