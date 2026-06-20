#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class BurningShipRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    BurningShipRenderer();
    void Render() override;
};
