#pragma once
#include "../Renderer.h"

/*
* @class RenderFixedPoint1
* @brief Threaded FixedPoint1 rendering routines.
*/
class FixedPoint1Renderer : public Renderer
{
    double _minStep;

public:
    FixedPoint1Renderer();

    void Render() override;
    void SetParams(double minStep);
};
