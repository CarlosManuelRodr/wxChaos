#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint1Renderer.h"

/*
* @class FixedPoint1
* @brief Handles the RenderFixedPoint1 threads.
*/
class FixedPoint1 : public Fractal
{
    FixedPoint1Renderer* myRender;
    double minStep;
public:
    FixedPoint1(unsigned int width, unsigned int height);
    ~FixedPoint1() override;
    wxString GetName() const override { return wxT("Fixed Point: sin(z)"); }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
