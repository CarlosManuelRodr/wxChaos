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

public:
    ManowarJuliaRenderer();

    void Render() override;
};
