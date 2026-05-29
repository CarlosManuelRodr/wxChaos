#pragma once
#include "../Fractal.h"
#include "../renderers/MandelbrotRenderer.h"

/*
* @class Mandelbrot
* @brief Handles the RenderMandelbrot threads.
*/
class Mandelbrot : public Fractal
{
    MandelbrotRenderer* myRender;
    int buddhaRandomP;
public:
    Mandelbrot(int width, int height);
    ~Mandelbrot() override;

    void Render() override;     ///< Launch threads.
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PreRender() override;
    void PreDrawMaps() override;
};
