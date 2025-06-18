#pragma once
#include "../Fractal.h"
#include "../renderers/RenderJuliaZN.h"

/*
* @class JuliaZN
* @brief Handles the RenderJuliaZN threads.
*/
class JuliaZN: public Fractal
{
    RenderJuliaZN* myRender;
    int n;
    double bailout;
public:
    JuliaZN(sf::RenderWindow *Window);
    JuliaZN(int width, int height);
    ~JuliaZN();

    void CopyOptFromPanel();
    void Render();
    void DrawOrbit();
};
