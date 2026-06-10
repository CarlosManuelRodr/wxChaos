#pragma once
#include "../Renderer.h"

/*
* @class RenderSinoidal
* @brief Threaded Sinoidal rendering routines.
*/
class SinoidalRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (SinoidalRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();

public:
    SinoidalRenderer();

    void Render() override;
};
