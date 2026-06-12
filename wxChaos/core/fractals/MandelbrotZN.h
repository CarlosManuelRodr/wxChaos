#pragma once
#include "../Fractal.h"
#include "../renderers/MandelbrotZNRenderer.h"

/*
* @class MandelbrotZN
* @brief Handles the RenderMandelbrotZN threads.
*/
class MandelbrotZN : public Fractal
{
private:
    int n;
    double bailout;
    MandelbrotZNRenderer* myRender;
public:
    MandelbrotZN(unsigned int width, unsigned int height);
    ~MandelbrotZN() override;
    wxString GetName() const override { return wxT("Mandelbrot ZN"); }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
