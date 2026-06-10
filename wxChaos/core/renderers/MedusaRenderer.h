#pragma once
#include "../Renderer.h"

/*
* @class RenderMedusa
* @brief Threaded Medusa rendering routines.
*/
class MedusaRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    MedusaRenderer();

    void Render() override;
};
