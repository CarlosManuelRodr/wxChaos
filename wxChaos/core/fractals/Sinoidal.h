#pragma once
#include "../Fractal.h"
#include "../renderers/RenderSinoidal.h"

/*
* @class Sinoidal
* @brief Handles the RenderSinoidal threads.
*/
class Sinoidal : public Fractal
{
    RenderSinoidal* myRender;
public:
    explicit Sinoidal(const sf::RenderWindow* window);
    Sinoidal(int width, int height);
    ~Sinoidal() override;

    void Render() override;
    void DrawOrbit() override;
};
