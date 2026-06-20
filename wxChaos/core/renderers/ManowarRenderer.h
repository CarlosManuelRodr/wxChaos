#pragma once
#include "../Renderer.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class ManowarRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    ManowarRenderer();
    void Render() override;
};
