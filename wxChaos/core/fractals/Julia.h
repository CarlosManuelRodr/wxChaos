#pragma once
#include "../Fractal.h"
#include "../renderers/JuliaRenderer.h"

/*
* @class Julia
* @brief Handles the RenderJulia threads.
*/
class Julia: public Fractal
{
    JuliaRenderer* myRender;
public:
    Julia(const sf::RenderWindow* window);
    Julia(int width, int height);
    ~Julia();

    void Render();
    void DrawOrbit();
};
