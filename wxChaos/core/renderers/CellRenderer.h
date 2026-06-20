#pragma once
#include "../Renderer.h"

/*
* @class RenderCell
* @brief Threaded Cell rendering routines.
*/
class CellRenderer : public Renderer
{
    double _bailout;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    CellRenderer();

    void Render() override;
    void SetParams(double bailout);
};
