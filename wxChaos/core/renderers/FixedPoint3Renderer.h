#pragma once
#include "../Renderer.h"

/*
* @class RenderFixedPoint3
* @brief Threaded FixedPoint3 rendering routines.
*/
class FixedPoint3Renderer : public Renderer
{
    double _minStep;

public:
    FixedPoint3Renderer();

    void Render() override;
    void SetParams(double minStep);
};
