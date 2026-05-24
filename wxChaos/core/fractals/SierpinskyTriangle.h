#pragma once
#include "../Fractal.h"
#include "../renderers/RenderSierpinskyTriangle.h"

/*
* @class SierpTriangle
* @brief Renders a Sierpinsky triangle.
*/
class SierpinskyTriangle : public Fractal
{
    RenderSierpinskyTriangle* myRender;
public:
    explicit SierpinskyTriangle(sf::RenderWindow* Window);
    SierpinskyTriangle(int width, int height);

    void Render() override;
};
