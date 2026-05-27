#pragma once
#include "../Fractal.h"
#include "../renderers/ManowarRenderer.h"

/*
* @class Manowar
* @brief Handles the RenderManowar threads.
*/
class Manowar : public Fractal
{
    ManowarRenderer* myRender;
public:
    explicit Manowar(const sf::RenderWindow* window);
    Manowar(int width, int height);
    ~Manowar() override;

    void Render() override;
    void DrawOrbit() override;
};
