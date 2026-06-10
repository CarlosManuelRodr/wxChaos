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

public:
    ManowarRenderer();
    void Render() override;
};
