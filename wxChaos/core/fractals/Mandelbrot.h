#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class Mandelbrot
* @brief Handles the RenderMandelbrot threads.
*/
class Mandelbrot : public Fractal
{
private:
    RenderMandelbrot* myRender;
    int buddhaRandomP;
public:
    Mandelbrot(sf::RenderWindow* Window);
    Mandelbrot(int width, int height);
    ~Mandelbrot();

    void Render();     ///< Launch threads.
    void DrawOrbit();
    void CopyOptFromPanel();
    void PreRender();
    void PreDrawMaps();
};
