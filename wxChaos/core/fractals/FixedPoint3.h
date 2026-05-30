#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint3Renderer.h"

/*
* @class FixedPoint3
* @brief Handles the RenderFixedPoint3 threads.
*/
class FixedPoint3 : public Fractal
{
    FixedPoint3Renderer* myRender;
    double minStep;
public:
    FixedPoint3(unsigned int width, unsigned int height);
    ~FixedPoint3() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
