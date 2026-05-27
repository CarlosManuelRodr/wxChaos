#pragma once
#include "../Fractal.h"
#include "../renderers/MedusaRenderer.h"

/*
* @class Medusa
* @brief Handles the RenderMedusa threads.
*/
class Medusa : public Fractal
{
    MedusaRenderer* myRender;
public:
    explicit Medusa(const sf::RenderWindow* window);
    Medusa(int width, int height);
    ~Medusa() override;

    void Render() override;
    void DrawOrbit() override;
};
