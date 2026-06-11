#pragma once
#include "../Renderer.h"

/*
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class CellRenderer : public Renderer
{
    double _bailout;

    template<class MeasurePoint>
    Point TracePoint(double pixelRe, double pixelIm, MeasurePoint measure) const;

public:
    CellRenderer();

    void Render() override;
    void SetParams(double bailout);
};
