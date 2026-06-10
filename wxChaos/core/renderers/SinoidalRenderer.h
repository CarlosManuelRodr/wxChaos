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

public:
    SinoidalRenderer();

    void Render() override;
};
