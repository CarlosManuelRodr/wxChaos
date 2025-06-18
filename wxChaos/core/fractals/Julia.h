#pragma once
#include "../FractalClasses.h"
#include "../renderers/RenderJulia.h"

/*
* @class Julia
* @brief Handles the RenderJulia threads.
*/
class Julia: public Fractal
{
    RenderJulia* myRender;
public:
    Julia(sf::RenderWindow* Window);
    Julia(int width, int height);
    ~Julia();

    void Render();
    void DrawOrbit();
};
