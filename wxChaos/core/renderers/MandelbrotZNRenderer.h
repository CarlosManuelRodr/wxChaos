#pragma once
#include "PerturbationRenderer.h"

/*
* @class RenderMandelbrotZN
* @brief Threaded MandelbrotZN rendering routines.
*/
class MandelbrotZNRenderer : public PerturbationRenderer
{
    int _n;
    double _bailout;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;
    [[nodiscard]] PerturbationReference BuildPerturbationReference(const HighPrecisionReal& centerRe, const HighPrecisionReal& centerIm) const;
    template<class MeasurePoint>
    PerturbationTraceResult TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                   const PerturbationReference& reference, MeasurePoint measure) const;

public:
    MandelbrotZNRenderer();

    void Render() override;
    void SetParams(int n, double bailout);
};
