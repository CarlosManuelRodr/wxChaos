#pragma once
#include "../Fractal.h"
#include "../renderers/RenderMedusa.h"

/*
* @class Medusa
* @brief Handles the RenderMedusa threads.
*/
class Medusa : public Fractal
{
    RenderMedusa* myRender;
public:
    explicit Medusa(sf::RenderWindow* window);
    Medusa(int width, int height);
    ~Medusa() override;

    void Render() override;
    void DrawOrbit() override;
};
