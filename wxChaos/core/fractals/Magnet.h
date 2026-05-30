#pragma once
#include "../Fractal.h"
#include "../renderers/MagnetRenderer.h"

/*
* @class Magnet
* @brief Handles the RenderMagnet threads.
*/

class Magnet : public Fractal
{
    MagnetRenderer* myRender;
public:
    Magnet(unsigned int width, unsigned int height);
    ~Magnet() override;

    void Render() override;
    void DrawOrbit() override;
};
