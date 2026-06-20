// ReSharper disable CppTooWideScope
#include "TricornRenderer.h"

TricornRenderer::TricornRenderer() = default;

template<class Real, class MeasurePoint>
Renderer::Point TricornRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    Real zRe = 0;
    Real zIm = 0;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real zRe2 = zRe * zRe;
        const Real zIm2 = zIm * zIm;
        const Real squaredRe = zRe2 - zIm2;
        const Real squaredIm = Real(-2) * zRe * zIm;

        zRe = squaredRe + pixelRe;
        zIm = squaredIm + pixelIm;
        const Real zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = ToDouble(zRe);
        point.zIm = ToDouble(zIm);
        point.zNorm = ToDouble(zNorm);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(squaredRe), ToDouble(squaredIm), wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;

            if (zNorm > Real(4))
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = n;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, n, point.zRe, point.zIm, point.zNorm,
                        ToDouble(squaredRe), ToDouble(squaredIm), wasInside);
            }
        }

        if (escaped && zNorm > Real(16) && !point.measureGaussianAfterEscape)
            break;
    }

    return point;
}

void TricornRenderer::Render()
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
