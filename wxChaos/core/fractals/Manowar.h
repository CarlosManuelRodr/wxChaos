#pragma once
#include "../Fractal.h"
#include "../renderers/RenderManowar.h"

/*
* @class Manowar
* @brief Handles the RenderManowar threads.
*/
class Manowar : public Fractal
{
    RenderManowar* myRender;
public:
    explicit Manowar(const sf::RenderWindow* window);
    Manowar(int width, int height);
    ~Manowar() override;

    void Render() override;
    void DrawOrbit() override;
};
