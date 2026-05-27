#pragma once
#include "../Fractal.h"
#include "../renderers/JuliaZNRenderer.h"

/*
* @class JuliaZN
* @brief Handles the RenderJuliaZN threads.
*/
class JuliaZN: public Fractal
{
    JuliaZNRenderer* myRender;
    int n;
    double bailout;
public:
    explicit JuliaZN(const sf::RenderWindow* window);
    JuliaZN(int width, int height);
    ~JuliaZN() override;

    void CopyOptFromPanel() override;
    void Render() override;
    void DrawOrbit() override;
};
