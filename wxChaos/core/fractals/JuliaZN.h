#pragma once
#include "../Fractal.h"
#include "../renderers/JuliaZNRenderer.h"

/*
* @class JuliaZN
* @brief Handles the RenderJuliaZN threads.
*/
class JuliaZN: public Fractal
{
    JuliaZNRenderer* myRender;
    int n;
    double bailout;
public:
    JuliaZN(unsigned int width, unsigned int height);
    ~JuliaZN() override;
    wxString GetName() const override { return wxT("Julia ZN"); }

    void CopyOptFromPanel() override;
    void Render() override;
    void DrawOrbit() override;
};
