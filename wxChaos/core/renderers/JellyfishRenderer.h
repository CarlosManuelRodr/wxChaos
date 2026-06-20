#pragma once
#include "../Renderer.h"

/*
* @class JellyfishRenderer
* @brief Threaded Jellyfish rendering routines.
*/
class JellyfishRenderer : public Renderer
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    JellyfishRenderer();

    void Render() override;
};
