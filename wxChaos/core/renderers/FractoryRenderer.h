#pragma once
#include "../Renderer.h"

/*
* @class RenderFractory
* @brief Threaded Fractory rendering routines.
*/
class FractoryRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    FractoryRenderer();
    void Render() override;
};
