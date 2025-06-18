#pragma once
#include "../Fractal.h"
#include "../renderers/RenderBurningShip.h"

/*
* @class BurningShip
* @brief Handles the RenderBurningShip threads.
*/
class BurningShip : public Fractal
{
private:
    RenderBurningShip* myRender;
public:
    BurningShip(sf::RenderWindow* Window);
    BurningShip(int width, int height);
    ~BurningShip();

    void Render();
    void DrawOrbit();
};
