#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"

/*
* @class Cell
* @brief Handles the RenderCell threads.
*/
class Cell : public Fractal
{
private:
    RenderCell* myRender;
    double bailout;
public:
    Cell(sf::RenderWindow* Window);
    Cell(int width, int height);
    ~Cell();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
