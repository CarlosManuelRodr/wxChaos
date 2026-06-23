#pragma once
#include "PerturbationRenderer.h"

/*
* @class RenderJulia
* @brief Threaded Julia rendering routines.
*/
class JuliaRenderer : public PerturbationRenderer
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
