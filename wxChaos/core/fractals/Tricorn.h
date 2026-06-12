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
    Tricorn(unsigned int width, unsigned int height);
    ~Tricorn();
    wxString GetName() const override { return wxT("Tricorn"); }

    void Render();
    void DrawOrbit();
};
