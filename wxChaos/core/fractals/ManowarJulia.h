#pragma once
#include "../Fractal.h"
#include "../renderers/ManowarJuliaRenderer.h"

/*
* @class ManowarJulia
* @brief Handles the RenderManowarJulia threads.
*/
class ManowarJulia : public Fractal
{
    ManowarJuliaRenderer* myRender;
public:
    ManowarJulia(unsigned int width, unsigned int height);
    ~ManowarJulia() override;

    void Render() override;
    void DrawOrbit() override;
};
