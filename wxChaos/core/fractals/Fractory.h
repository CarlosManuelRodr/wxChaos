#pragma once
#include "../Fractal.h"
#include "../renderers/FractoryRenderer.h"

/*
* @class Fractory
* @brief Handles the RenderFractory threads.
*/
class Fractory : public Fractal
{
private:
    FractoryRenderer *myRender;
public:
    Fractory(unsigned int width, unsigned int height);
    ~Fractory();

    void Render();
    void DrawOrbit();
};
