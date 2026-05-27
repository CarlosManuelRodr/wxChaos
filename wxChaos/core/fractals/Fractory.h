#pragma once
#include "../Fractal.h"
#include "../renderers/RenderFractory.h"

/*
* @class Fractory
* @brief Handles the RenderFractory threads.
*/
class Fractory : public Fractal
{
private:
    RenderFractory *myRender;
public:
    Fractory(const sf::RenderWindow* window);
    Fractory(int width, int height);
    ~Fractory();

    void Render();
    void DrawOrbit();
};
