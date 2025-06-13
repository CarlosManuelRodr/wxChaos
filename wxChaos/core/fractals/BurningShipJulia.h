#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

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
