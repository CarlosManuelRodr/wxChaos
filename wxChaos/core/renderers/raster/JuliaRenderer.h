#pragma once
#include "PerturbationRenderWorker.h"

/**
* @class JuliaRenderer
* @brief Renders quadratic Julia samples for one pixel region.
*
* The renderer owns the Julia formula trace, supports the perturbation path for
* high-precision views, and dispatches compatible coloring algorithms through
* the shared Renderer paths.
*/
class JuliaRenderer : public PerturbationRenderWorker
{
    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;
    [[nodiscard]] PerturbationReference BuildPerturbationReference(const HighPrecisionReal& centerRe, const HighPrecisionReal& centerIm) const;
    template<class MeasurePoint>
    PerturbationTraceResult TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                   const PerturbationReference& reference, MeasurePoint measure) const;

public:
    JuliaRenderer();
    void Render() override;
};
