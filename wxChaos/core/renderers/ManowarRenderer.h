#pragma once
#include "../Renderer.h"

/*
* @class RenderManowar
* @brief Threaded Manowar rendering routines.
*/
class ManowarRenderer : public Renderer
{
    void EscapeTimeRender();
    EscapePoint IterateEscapePoint(double pixelRe, double pixelIm) const;
    unsigned int ColorEscapePoint(const EscapePoint& point) const;
    void GaussianIntRender();
    GaussianIntegerPoint IterateGaussianIntegerPoint(double pixelRe, double pixelIm) const;
    void EscapeAngleRender();
    EscapePoint IterateEscapeAnglePoint(double pixelRe, double pixelIm) const;

public:
    ManowarRenderer();
    void Render() override;
};
