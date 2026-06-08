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

    void EscapeTimeRender();
    EscapePoint IterateEscapePoint(double pixelRe, double pixelIm) const;
    unsigned int ColorEscapePoint(const EscapePoint& point) const;
    void GaussianIntRender();
    GaussianIntegerPoint IterateGaussianIntegerPoint(double pixelRe, double pixelIm) const;
    void EscapeAngleRender();
    EscapePoint IterateEscapeAnglePoint(double pixelRe, double pixelIm) const;

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
