#pragma once
#include "../Renderer.h"

/*
* @class RenderManowarJulia
* @brief Threaded ManowarJulia rendering routines.
*/
class ManowarJuliaRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (ManowarJuliaRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();

public:
    ManowarJuliaRenderer();

    void Render() override;
};
