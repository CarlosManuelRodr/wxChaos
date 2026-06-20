#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class BurningShipJuliaRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    BurningShipJuliaRenderer();
    void Render() override;
};
