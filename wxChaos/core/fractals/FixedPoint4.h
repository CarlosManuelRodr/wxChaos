#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class FixedPoint4
* @brief Handles the RenderFixedPoint4 threads.
*/
class FixedPoint4 : public Fractal
{
private:
    RenderFixedPoint4* myRender;
    double minStep;
public:
    FixedPoint4(sf::RenderWindow* Window);
    FixedPoint4(int width, int height);
    ~FixedPoint4();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
