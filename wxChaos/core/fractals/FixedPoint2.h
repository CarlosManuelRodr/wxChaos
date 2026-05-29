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
    FixedPoint2(int width, int height);
    ~FixedPoint2() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
