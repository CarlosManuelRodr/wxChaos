#pragma once
#include "../Fractal.h"
#include "../renderers/TricornRenderer.h"

/*
* @class Tricorn
* @brief Handles the RenderTricorn threads.
*/
class Tricorn : public Fractal
{
private:
    TricornRenderer* myRender;
public:
    Tricorn(int width, int height);
    ~Tricorn();

    void Render();
    void DrawOrbit();
};
