#pragma once
#include "../Fractal.h"
#include "../renderers/RenderMagnet.h"

/*
* @class Magnet
* @brief Handles the RenderMagnet threads.
*/

class Magnet : public Fractal
{
    RenderMagnet* myRender;
public:
    explicit Magnet(const sf::RenderWindow* window);
    Magnet(int width, int height);
    ~Magnet() override;

    void Render() override;
    void DrawOrbit() override;
};
