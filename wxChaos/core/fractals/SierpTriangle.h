#pragma once
#include "../Fractal.h"
#include "../renderers/RenderSierpTriangle.h"

/*
* @class SierpTriangle
* @brief Renders a Sierpinsky triangle.
*/
class SierpTriangle : public Fractal
{
private:
    RenderSierpTriangle* myRender;
public:
    SierpTriangle(sf::RenderWindow* Window);
    SierpTriangle(int width, int height);

    void Render();
};
