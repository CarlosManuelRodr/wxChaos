#pragma once
#include "../Renderer.h"

/*
* @class RenderFixedPoint4
* @brief Threaded FixedPoint4 rendering routines.
*/
class FixedPoint4Renderer : public Renderer
{
    double _minStep;

public:
    FixedPoint4Renderer();

    void Render() override;
    void SetParams(double minStep);
};
