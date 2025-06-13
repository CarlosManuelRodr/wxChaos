#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class FixedPoint2
* @brief Handles the RenderFixedPoint2 threads.
*/
class FixedPoint2 : public Fractal
{
private:
    RenderFixedPoint2* myRender;
    double minStep;
public:
    FixedPoint2(sf::RenderWindow* Window);
    FixedPoint2(int width, int height);
    ~FixedPoint2();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
