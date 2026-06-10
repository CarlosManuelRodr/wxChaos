#pragma once
#include "../Renderer.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class ManowarRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (ManowarRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();

public:
    ManowarRenderer();
    void Render() override;
};
