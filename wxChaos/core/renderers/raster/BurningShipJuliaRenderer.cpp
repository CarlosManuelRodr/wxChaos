// ReSharper disable CppTooWideScope
#include "BurningShipJuliaRenderer.h"

BurningShipJuliaRenderer::BurningShipJuliaRenderer() = default;

template<class Real, class MeasurePoint>
RasterRenderWorker::Point BurningShipJuliaRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    point.zRe = point.startRe;
    point.zIm = point.startIm;
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    Real zRe = pixelRe;
    Real zIm = pixelIm;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIterations; n++)
    {
        const Real currentNorm = zRe * zRe + zIm * zIm;
        point.zNorm = ToDouble(currentNorm);
        if (n > 0 && !escaped && currentNorm > Real(4))
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = ToDouble(zRe);
            point.escapedZIm = ToDouble(zIm);
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && currentNorm > Real(16) && !point.measureGaussianAfterEscape)
            break;

        const Real zRe2 = zRe * zRe;
        const Real zIm2 = zIm * zIm;
        const Real squaredRe = zRe2 - zIm2;
        const Real squaredIm = Real(2) * RealAbs(zRe) * RealAbs(zIm);

        zRe = squaredRe + Real(_kReal);
        zIm = squaredIm + Real(_kImaginary);
        const Real zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = ToDouble(zRe);
        point.zIm = ToDouble(zIm);
        point.zNorm = ToDouble(zNorm);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(squaredRe), ToDouble(squaredIm), wasInside);

        if (!escaped)
            point.iterations = n + 1;
    }

    return point;
}

void BurningShipJuliaRenderer::Render()
{
    const auto tracePoint = [this](const auto& pixelRe, const auto& pixelIm, auto measure)
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
        case RenderingAlgorithmType::TriangleInequality:
            TriangleInequalityRender(tracePoint);
            break;
        default:
            break;
    }
}
