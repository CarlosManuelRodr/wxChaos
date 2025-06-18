#pragma once
#include "../FractalClasses.h"
#include "../renderers/RenderFixedPoint3.h"

/*
* @class FixedPoint3
* @brief Handles the RenderFixedPoint3 threads.
*/
class FixedPoint3 : public Fractal
{
private:
    RenderFixedPoint3* myRender;
    double minStep;
public:
    FixedPoint3(sf::RenderWindow* Window);
    FixedPoint3(int width, int height);
    ~FixedPoint3();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
