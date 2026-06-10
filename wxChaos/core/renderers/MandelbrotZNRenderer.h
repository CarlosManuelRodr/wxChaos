#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class MandelbrotZNRenderer : public Renderer
{
    int _n;
    double _bailout;

    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
