#pragma once
#include "PerturbationRenderer.h"

/*
* @class RenderMandelbrot
* @brief Threaded Mandelbrot rendering routines.
*/
class MandelbrotRenderer : public PerturbationRenderer
{
    int _buddhaRandomP;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;
    [[nodiscard]] PerturbationReference BuildPerturbationReference(const HighPrecisionReal& centerRe, const HighPrecisionReal& centerIm) const;
    template<class MeasurePoint>
    PerturbationTraceResult TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                   const PerturbationReference& reference, MeasurePoint measure) const;
    void BuddhabrotRender();

public:
    MandelbrotRenderer();

    void Render() override;
    unsigned int GetProgress() override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
