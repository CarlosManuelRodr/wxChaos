#pragma once
#include "../raster/RasterRenderWorker.h"

/**
* @class CellRenderer
* @brief Renders the Cell escape-time recurrence for one pixel region.
*
* The renderer owns the formula trace for Cell and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class CellRenderer : public RasterRenderWorker
{
    double _bailout;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    CellRenderer();

    void Render() override;
    void SetParams(double bailout);
};
