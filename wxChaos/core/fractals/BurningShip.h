#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

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
