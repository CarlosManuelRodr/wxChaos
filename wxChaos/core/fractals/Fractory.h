#pragma once
#include "../FractalClasses.h"
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
    Fractory(sf::RenderWindow* Window);
    Fractory(int width, int height);
    ~Fractory();

    void Render();
    void DrawOrbit();
};
