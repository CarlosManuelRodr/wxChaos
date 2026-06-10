#pragma once
#include "../Renderer.h"

/*
* @class RenderJuliaZN
* @brief Threaded JuliaZN rendering routines.
*/
class JuliaZNRenderer : public Renderer
{
    int _n;
    double _bailout;

    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    JuliaZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
