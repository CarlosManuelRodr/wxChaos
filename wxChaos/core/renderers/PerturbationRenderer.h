#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include "../Renderer.h"

class PerturbationRenderer : public Renderer
{
protected:
    struct PerturbationReference
    {
        HighPrecisionReal centerRe;
        HighPrecisionReal centerIm;
        double centerReDouble = 0.0;
        double centerImDouble = 0.0;
        std::vector<PrecisionComplex<double>> orbit;
    };

    struct PerturbationTraceResult
    {
        Point point;
        bool valid = true;
    };

    [[nodiscard]] bool ShouldUsePerturbationRender() const;
    [[nodiscard]] static bool HasPerturbationGlitchOrDiverged(double referenceNorm, double zNorm, bool escaped,
                                                              double boundedNorm = 4.0, double referenceDivergedNorm = 16.0);

    template<class PixelRenderer>
    void RenderPerturbationPixels(PixelRenderer pixelRenderer);
    template<class BuildReference, class TracePerturbationPoint, class TracePrecisePoint, class ColorPoint, class MeasurePoint>
    void PerturbationRenderFromPoint(BuildReference buildReference, TracePerturbationPoint tracePerturbationPoint,
                                     TracePrecisePoint tracePrecisePoint, ColorPoint colorPoint, MeasurePoint measure);
    template<class RenderPointFn>
    void PerturbationEscapeTimeRender(RenderPointFn renderPointFn);
    template<class RenderPointFn>
    void PerturbationGaussianIntRender(RenderPointFn renderPointFn);
    template<class RenderPointFn>
    void PerturbationEscapeAngleRender(RenderPointFn renderPointFn);
    template<class RenderPointFn>
    void PerturbationTriangleInequalityRender(RenderPointFn renderPointFn);
};

inline bool PerturbationRenderer::ShouldUsePerturbationRender() const
{
    if (_renderingPrecisionMode == RenderingPrecisionMode::Fast)
        return true;

    return _renderingPrecisionMode == RenderingPrecisionMode::Adaptative && _useHighPrecision;
}

inline bool PerturbationRenderer::HasPerturbationGlitchOrDiverged(const double referenceNorm, const double zNorm, const bool escaped,
                                                                  const double boundedNorm, const double referenceDivergedNorm)
{
    if (!std::isfinite(referenceNorm) || !std::isfinite(zNorm))
        return true;

    if (!escaped && zNorm <= boundedNorm && referenceNorm > referenceDivergedNorm)
        return true;

    constexpr double glitchThreshold = 1.0e-6;
    return referenceNorm > 1.0e-12 && zNorm < referenceNorm * glitchThreshold;
}

template<class PixelRenderer>
void PerturbationRenderer::RenderPerturbationPixels(PixelRenderer pixelRenderer)
{
    const unsigned int precisionBits = std::max(_highPrecisionBits, 128U);
    HighPrecisionReal::PrecisionScope precision(precisionBits);
    const HighPrecisionReal top = HighPrecisionReal::WithCurrentPrecision(_preciseView.top);
    const HighPrecisionReal left = HighPrecisionReal::WithCurrentPrecision(_preciseView.left);
    const HighPrecisionReal xFactor = HighPrecisionReal::WithCurrentPrecision(_preciseXFactor);
    const HighPrecisionReal yFactor = HighPrecisionReal::WithCurrentPrecision(_preciseYFactor);
    HighPrecisionReal pixelIm = top - HighPrecisionReal(_heightOrigin) * yFactor;

    for (_y = _heightOrigin; _y < _heightFinal; _y++)
    {
        HighPrecisionReal pixelRe = left + HighPrecisionReal(_widthOrigin) * xFactor;
        for (_x = _widthOrigin; _x < _widthFinal; _x++)
        {
            pixelRenderer(pixelRe, pixelIm);
            pixelRe += xFactor;
        }
        pixelIm -= yFactor;
    }
}

template<class BuildReference, class TracePerturbationPoint, class TracePrecisePoint, class ColorPoint, class MeasurePoint>
void PerturbationRenderer::PerturbationRenderFromPoint(BuildReference buildReference, TracePerturbationPoint tracePerturbationPoint,
                                                       TracePrecisePoint tracePrecisePoint, ColorPoint colorPoint, MeasurePoint measure)
{
    std::vector<decltype(buildReference(_preciseView.left, _preciseView.top))> references;
    references.push_back(buildReference((_preciseView.left + _preciseView.right) / HighPrecisionReal(2),
                                        (_preciseView.top + _preciseView.bottom) / HighPrecisionReal(2)));

    const auto renderPixel = [this, colorPoint, measure, &references, buildReference, tracePerturbationPoint, tracePrecisePoint]
        (const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm)
    {
        PerturbationTraceResult result = tracePerturbationPoint(pixelRe, pixelIm, references.back(), measure);
        if (!result.valid)
        {
            references.push_back(buildReference(pixelRe, pixelIm));
            result = tracePerturbationPoint(pixelRe, pixelIm, references.back(), measure);
        }

        const Point point = result.valid ? result.point : tracePrecisePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = colorPoint(point);
    };

    RenderPerturbationPixels(renderPixel);
}

template<class RenderPointFn>
void PerturbationRenderer::PerturbationEscapeTimeRender(RenderPointFn renderPointFn)
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        const auto colorPoint = [this](const Point& point) { return EscapeTimeColor(point); };
        renderPointFn(colorPoint, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    const auto colorPoint = [this](const Point& point) { return EscapeTimeColor(point); };
    renderPointFn(colorPoint, measure);
}

template<class RenderPointFn>
void PerturbationRenderer::PerturbationGaussianIntRender(RenderPointFn renderPointFn)
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                                const double zNorm, double, double, const bool wasInside)
        {
            MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
            MeasureOrbitTrap(point, event, zRe, zIm);
            MeasureEscapeMu(point, event, zNorm);
        };
        const auto colorPoint = [this](const Point& point) { return GaussianIntegerColor(point); };
        renderPointFn(colorPoint, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    const auto colorPoint = [this](const Point& point) { return GaussianIntegerColor(point); };
    renderPointFn(colorPoint, measure);
}

template<class RenderPointFn>
void PerturbationRenderer::PerturbationEscapeAngleRender(RenderPointFn renderPointFn)
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    const auto colorPoint = [this](const Point& point) { return EscapeAngleColor(point); };
    renderPointFn(colorPoint, measure);
}

template<class RenderPointFn>
void PerturbationRenderer::PerturbationTriangleInequalityRender(RenderPointFn renderPointFn)
{
    const auto measure = [](Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
                            const double zNorm, const double squaredRe, const double squaredIm, const bool wasInside)
    {
        MeasureTriangleInequality(point, event, iteration, zRe, zIm, squaredRe, squaredIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    const auto colorPoint = [this](const Point& point) { return TriangleInequalityColor(point); };
    renderPointFn(colorPoint, measure);
}
