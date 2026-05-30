#pragma once
#include "../Fractal.h"
#include "../renderers/SierpinskyTriangleRenderer.h"

/*
* @class SierpTriangle
* @brief Renders a Sierpinsky triangle.
*/
class SierpinskyTriangle : public Fractal
{
    SierpinskyTriangleRenderer* myRender;
public:
    SierpinskyTriangle(unsigned int width, unsigned int height);

    void Render() override;
};
