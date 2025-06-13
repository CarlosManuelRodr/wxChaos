#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class RenderMandelbrot : public RenderFractal
{
private:
    int buddhaRandomP;
    int bd;
public:
    RenderMandelbrot();

    void Render();
    void SpecialRender();
    int AskProgress();

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
