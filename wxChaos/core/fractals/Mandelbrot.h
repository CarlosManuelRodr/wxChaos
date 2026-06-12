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
    Mandelbrot(unsigned int width, unsigned int height);
    ~Mandelbrot() override;
    wxString GetName() const override { return wxT("Mandelbrot"); }

    void Render() override;     ///< Launch threads.
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PreRender() override;
    void PreDrawMaps() override;
};
