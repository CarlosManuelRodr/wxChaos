#pragma once
#include "../Fractal.h"
#include "../renderers/SinoidalRenderer.h"

/*
* @class Sinoidal
* @brief Handles the RenderSinoidal threads.
*/
class Sinoidal : public Fractal
{
    SinoidalRenderer* myRender;
public:
    Sinoidal(unsigned int width, unsigned int height);
    ~Sinoidal() override;

    void Render() override;
    void DrawOrbit() override;
};
