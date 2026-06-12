#pragma once
#include "../Fractal.h"
#include "../renderers/JellyfishRenderer.h"

/*
* @class Jellyfish
* @brief Handles the JellyfishRenderer threads.
*/
class Jellyfish : public Fractal
{
    JellyfishRenderer* myRender;
public:
    Jellyfish(unsigned int width, unsigned int height);
    ~Jellyfish() override;
    wxString GetName() const override { return wxT("Jellyfish"); }

    void Render() override;
    void DrawOrbit() override;
};
