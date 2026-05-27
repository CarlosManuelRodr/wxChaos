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
    explicit JuliaZN(const sf::RenderWindow* window);
    JuliaZN(int width, int height);
    ~JuliaZN() override;

    void CopyOptFromPanel() override;
    void Render() override;
    void DrawOrbit() override;
};
