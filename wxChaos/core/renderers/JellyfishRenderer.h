#pragma once
#include "../Renderer.h"

/*
* @class JellyfishRenderer
* @brief Threaded Jellyfish rendering routines.
*/
class JellyfishRenderer : public Renderer
{
    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    JellyfishRenderer();

    void Render() override;
};
