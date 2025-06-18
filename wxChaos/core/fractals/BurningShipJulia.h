#pragma once
#include "../Fractal.h"
#include "../renderers/RenderBurningShipJulia.h"

/*
* @class BurningShipJulia
* @brief Handles the RenderBurningShipJulia threads.
*/
class BurningShipJulia : public Fractal
{
private:
    RenderBurningShipJulia *myRender;
public:
    BurningShipJulia(sf::RenderWindow* Window);
    BurningShipJulia(int width, int height);
    ~BurningShipJulia();

    void Render();
    void DrawOrbit();
};
