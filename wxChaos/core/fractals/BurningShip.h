#pragma once
#include "../Fractal.h"
#include "../renderers/RenderBurningShip.h"

/*
* @class BurningShip
* @brief Handles the RenderBurningShip threads.
*/
class BurningShip : public Fractal
{
    RenderBurningShip* myRender;
public:
    explicit BurningShip(sf::RenderWindow* Window);
    BurningShip(int width, int height);
    ~BurningShip() override;

    void Render() override;
    void DrawOrbit() override;
};
