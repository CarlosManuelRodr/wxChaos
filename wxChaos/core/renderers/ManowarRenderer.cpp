#include "ManowarRenderer.h"

ManowarRenderer::ManowarRenderer() = default;

template<class Real, class MeasurePoint>
Renderer::Point ManowarRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    Real zRe = pixelRe;
    Real zIm = pixelIm;
    Real manRe = pixelRe;
    Real manIm = pixelIm;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
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
        const Real tempRe = zRe;
        const Real tempIm = zIm;

        zIm = Real(2) * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;
        const Real zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = ToDouble(zRe);
        point.zIm = ToDouble(zIm);
        point.zNorm = ToDouble(zNorm);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(zRe2 - zIm2), ToDouble(Real(2) * tempRe * tempIm), wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

void ManowarRenderer::Render()
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
