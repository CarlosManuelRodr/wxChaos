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
    Medusa(unsigned int width, unsigned int height);
    ~Medusa() override;

    void Render() override;
    void DrawOrbit() override;
};
