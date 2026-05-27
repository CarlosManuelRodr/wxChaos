#pragma once
#include "../Renderer.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class MandelbrotZNRenderer : public Renderer
{
    int _n;
    double _bailout;
public:
    MandelbrotZNRenderer();

    void Render() override;
    void SpecialRender() override;
    void SetParams(int n, double bailout);
};
