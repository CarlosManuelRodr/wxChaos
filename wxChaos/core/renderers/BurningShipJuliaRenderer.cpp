// ReSharper disable CppTooWideScope
#include <cmath>
#include "BurningShipJuliaRenderer.h"

BurningShipJuliaRenderer::BurningShipJuliaRenderer() = default;

template<class MeasurePoint>
Renderer::Point BurningShipJuliaRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.zRe = pixelRe;
    point.zIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    double zRe = pixelRe;
    double zIm = pixelIm;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zNorm = zRe * zRe + zIm * zIm;
        if (!escaped && point.zNorm > 4)
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = zRe;
            point.escapedZIm = zIm;
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, zRe, zIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && point.zNorm > 16 && !point.measureGaussianAfterEscape)
            break;

        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double squaredRe = zRe2 - zIm2;
        const double squaredIm = 2.0 * std::abs(zRe) * std::abs(zIm);

        zRe = squaredRe + _kReal;
        zIm = squaredIm + _kImaginary;
        const double zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = zRe;
        point.zIm = zIm;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, zRe, zIm, zNorm, squaredRe, squaredIm, wasInside);

        if (!escaped)
            point.iterations = n + 1;
    }

    return point;
}

void BurningShipJuliaRenderer::Render()
{
    const auto tracePoint = [this](const double pixelRe, const double pixelIm, auto measure)
    {
        return TracePoint(pixelRe, pixelIm, measure);
    };

    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender(tracePoint);
            break;
        case RenderingAlgorithmType::GaussianInt:
            GaussianIntRender(tracePoint);
            break;
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender(tracePoint);
            break;
        default:
            break;
    }
}
