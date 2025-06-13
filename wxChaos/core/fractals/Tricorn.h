#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class Tricorn
* @brief Handles the RenderTricorn threads.
*/
class Tricorn : public Fractal
{
private:
    RenderTricorn* myRender;
public:
    Tricorn(sf::RenderWindow* Window);
    Tricorn(int width, int height);
    ~Tricorn();

    void Render();
    void DrawOrbit();
};
