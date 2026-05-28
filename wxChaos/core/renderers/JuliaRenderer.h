#pragma once
#include "../Renderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public Renderer
{
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    void TriangleInequalityRender();
    void EscapeTimeSmoothRender();
    void EscapeTimeWithOrbitTrapRender();

public:
    JuliaRenderer();
    void Render() override;
};
