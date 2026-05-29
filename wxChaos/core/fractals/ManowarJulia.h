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
    ManowarJulia(int width, int height);
    ~ManowarJulia() override;

    void Render() override;
    void DrawOrbit() override;
};
