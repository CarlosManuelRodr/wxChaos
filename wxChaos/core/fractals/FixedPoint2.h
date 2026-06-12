#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint2Renderer.h"

/*
* @class FixedPoint2
* @brief Handles the RenderFixedPoint2 threads.
*/
class FixedPoint2 : public Fractal
{
    FixedPoint2Renderer* myRender;
    double minStep;
public:
    FixedPoint2(unsigned int width, unsigned int height);
    ~FixedPoint2() override;
    wxString GetName() const override { return wxT("Fixed Point: cos(z)"); }

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
