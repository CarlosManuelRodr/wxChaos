#pragma once
#include "../Fractal.h"
#include "../renderers/CellRenderer.h"

/*
* @class Cell
* @brief Handles the RenderCell threads.
*/
class Cell : public Fractal
{
private:
    CellRenderer* myRender;
    double bailout;
public:
    Cell(const sf::RenderWindow* window);
    Cell(int width, int height);
    ~Cell();

    void Render();
    void DrawOrbit();
    void CopyOptFromPanel();
};
