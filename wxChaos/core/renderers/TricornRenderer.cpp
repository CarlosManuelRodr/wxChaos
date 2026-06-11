// ReSharper disable CppTooWideScope
#include "TricornRenderer.h"

TricornRenderer::TricornRenderer() = default;

template<class MeasurePoint>
Renderer::Point TricornRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    double zRe = 0.0;
    double zIm = 0.0;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double squaredRe = zRe2 - zIm2;
        const double squaredIm = -2.0 * zRe * zIm;

        zRe = squaredRe + pixelRe;
        zIm = squaredIm + pixelIm;
        const double zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = zRe;
        point.zIm = zIm;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, zRe, zIm, zNorm, squaredRe, squaredIm, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;

            if (zNorm > 4)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = n;
                point.escapedZRe = zRe;
                point.escapedZIm = zIm;
                point.escapedNorm = zNorm;
                measure(point, PointTraceEvent::Escaped, n, zRe, zIm, zNorm, squaredRe, squaredIm, wasInside);
            }
        }

        if (escaped && zNorm > 16 && !point.measureGaussianAfterEscape)
            break;
    }

    return point;
}

void TricornRenderer::Render()
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
