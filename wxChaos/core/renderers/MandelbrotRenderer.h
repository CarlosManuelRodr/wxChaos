#pragma once
#include "PerturbationRenderWorker.h"

/**
* @class MandelbrotRenderer
* @brief Renders Mandelbrot and Buddhabrot samples for one pixel region.
*
* The renderer owns the Mandelbrot formula trace, supports the perturbation
* path for high-precision views, and dispatches compatible coloring algorithms
* through the shared Renderer paths.
*/
class MandelbrotRenderer : public PerturbationRenderWorker
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
    [[nodiscard]] unsigned int GetProgress() const override;

    ///@brief Number of random points that will launch in Buddhabrot mode.
    ///@param n Number of points.
    void SetBuddhaRandomP(int n);
};
