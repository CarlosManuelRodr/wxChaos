#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFixedPoint1.h"

/*
* @class FixedPoint1
* @brief Handles the RenderFixedPoint1 threads.
*/
class FixedPoint1 : public Fractal
{
private:
    RenderFixedPoint1* myRender;
    double minStep;
public:
    FixedPoint1(sf::RenderWindow* Window);
    FixedPoint1(int width, int height);
    ~FixedPoint1();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
