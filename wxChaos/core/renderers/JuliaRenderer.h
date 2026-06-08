#pragma once
#include "../Renderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public Renderer
{
    void EscapeTimeRender();
    EscapePoint IterateEscapePoint(double pixelRe, double pixelIm) const;
    unsigned int ColorEscapePoint(const EscapePoint& point) const;
    void GaussianIntRender();
    GaussianIntegerPoint IterateGaussianIntegerPoint(double pixelRe, double pixelIm) const;
    void EscapeAngleRender();
    EscapePoint IterateEscapeAnglePoint(double pixelRe, double pixelIm) const;
    void TriangleInequalityRender();
    TriangleInequalityPoint IterateTriangleInequalityPoint(double pixelRe, double pixelIm) const;

public:
    JuliaRenderer();
    void Render() override;
};
