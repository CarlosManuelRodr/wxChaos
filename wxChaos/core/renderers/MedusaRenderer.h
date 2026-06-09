#pragma once
#include "../Renderer.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class MedusaRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;
    template<class MeasurePoint>
    void RenderFromPoint(unsigned int (MedusaRenderer::*colorPoint)(const Point&) const, MeasurePoint measure);
    void EscapeTimeRender();
    void GaussianIntRender();
    void EscapeAngleRender();
    unsigned int ColorEscapeTimePoint(const Point& point) const;
    unsigned int ColorGaussianIntegerPoint(const Point& point) const;
    unsigned int ColorEscapeAnglePoint(const Point& point) const;

public:
    MedusaRenderer();

    void Render() override;
};
