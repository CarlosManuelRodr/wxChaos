#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShip
* @brief Threaded BurningShip rendering routines.
*/
class BurningShipRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    BurningShipRenderer();
    void Render() override;
};
