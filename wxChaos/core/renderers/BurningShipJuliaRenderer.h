#pragma once
#include "../Renderer.h"

/*
* @class RenderBurningShipJulia
* @brief Threaded BurningShipJulia rendering routines.
*/
class BurningShipJuliaRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    BurningShipJuliaRenderer();
    void Render() override;
};
