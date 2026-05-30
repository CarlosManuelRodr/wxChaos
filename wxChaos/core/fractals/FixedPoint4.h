#pragma once
#include "../Fractal.h"
#include "../renderers/FixedPoint4Renderer.h"

/*
* @class FixedPoint4
* @brief Handles the RenderFixedPoint4 threads.
*/
class FixedPoint4 : public Fractal
{
    FixedPoint4Renderer* myRender;
    double minStep;
public:
    FixedPoint4(unsigned int width, unsigned int height);
    ~FixedPoint4() override;

    void Render() override;
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
};
