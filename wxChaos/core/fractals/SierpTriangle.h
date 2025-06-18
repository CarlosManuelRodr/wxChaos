#pragma once
#include "../FractalClasses.h"

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
