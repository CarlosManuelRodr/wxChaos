#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFixedPoint4.h"

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
