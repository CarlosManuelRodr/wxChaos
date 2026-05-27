#pragma once
#include "../Renderer.h"

/*
* @class RenderFixedPoint2
* @brief Threaded FixedPoint2 rendering routines.
*/
class FixedPoint2Renderer : public Renderer
{
    double _minStep;
public:
    FixedPoint2Renderer();

    void Render() override;
    void SetParams(double minStep);
};
