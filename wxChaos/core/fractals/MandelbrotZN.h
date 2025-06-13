#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class MandelbrotZN
* @brief Handles the RenderMandelbrotZN threads.
*/
class MandelbrotZN : public Fractal
{
private:
    int n;
    double bailout;
    RenderMandelbrotZN* myRender;
public:
    MandelbrotZN(sf::RenderWindow* Window);
    MandelbrotZN(int width, int height);
    ~MandelbrotZN();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
