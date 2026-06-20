#pragma once
#include "../Renderer.h"

/*
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class FractoryRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    FractoryRenderer();
    void Render() override;
};
