#pragma once
#include "../Fractal.h"
#include "../renderers/ManowarRenderer.h"

/*
* @class Manowar
* @brief Handles the RenderManowar threads.
*/
class Manowar : public Fractal
{
    ManowarRenderer* myRender;
public:
    Manowar(unsigned int width, unsigned int height);
    ~Manowar() override;
    wxString GetName() const override { return wxT("Manowar"); }

    void Render() override;
    void DrawOrbit() override;
};
