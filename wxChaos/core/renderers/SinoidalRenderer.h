#pragma once
#include "../Renderer.h"

/*
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class SinoidalRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void EscapeTimeWithOrbitTrapRender();

public:
    SinoidalRenderer();

    void Render() override;
};
