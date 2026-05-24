#pragma once
#include "../Fractal.h"
#include "../renderers/RenderMandelbrot.h"

/*
* @class Mandelbrot
* @brief Handles the RenderMandelbrot threads.
*/
class Mandelbrot : public Fractal
{
    RenderMandelbrot* myRender;
    int buddhaRandomP;
public:
    explicit Mandelbrot(sf::RenderWindow* window);
    Mandelbrot(int width, int height);
    ~Mandelbrot() override;

    void Render() override;     ///< Launch threads.
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PreRender() override;
    void PreDrawMaps() override;
};
