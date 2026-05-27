#pragma once
#include "../Fractal.h"
#include "../renderers/RenderJulia.h"

/*
* @class Julia
* @brief Handles the RenderJulia threads.
*/
class Julia: public Fractal
{
    RenderJulia* myRender;
public:
    Julia(const sf::RenderWindow* window);
    Julia(int width, int height);
    ~Julia();

    void Render();
    void DrawOrbit();
};
