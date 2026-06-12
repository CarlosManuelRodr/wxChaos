#pragma once
#include "../Fractal.h"
#include "../renderers/JuliaRenderer.h"

/*
* @class Julia
* @brief Handles the RenderJulia threads.
*/
class Julia: public Fractal
{
    JuliaRenderer* myRender;
public:
    Julia(unsigned int width, unsigned int height);
    ~Julia();
    wxString GetName() const override { return wxT("Julia"); }

    void Render();
    void DrawOrbit();
};
