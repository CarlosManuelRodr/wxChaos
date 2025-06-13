#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class Medusa
* @brief Handles the RenderMedusa threads.
*/
class Medusa : public Fractal
{
private:
    RenderMedusa* myRender;
public:
    Medusa(sf::RenderWindow* Window);
    Medusa(int width, int height);
    ~Medusa();

    void Render();
    void DrawOrbit();
};
