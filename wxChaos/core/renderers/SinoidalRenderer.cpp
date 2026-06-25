// ReSharper disable CppTooWideScope
#include "SinoidalRenderer.h"
using namespace std;

SinoidalRenderer::SinoidalRenderer() = default;

template<class Real, class MeasurePoint>
RenderWorker::Point SinoidalRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> constant{Real(_kReal), Real(_kImaginary)};
    PrecisionComplex<Real> z(pixelRe, pixelIm);
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real currentNorm = ComplexNorm(z);
        point.zNorm = ToDouble(currentNorm);
        if (!escaped && currentNorm > Real(_maxIter))
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = ToDouble(z.re);
            point.escapedZIm = ToDouble(z.im);
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && currentNorm > Real(_maxIter) * Real(_maxIter) && !point.measureGaussianAfterEscape)
            break;

        const PrecisionComplex<Real> transformedZ = constant * ComplexSin(z);
        z = transformedZ;

        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(ComplexNorm(z));
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(transformedZ.re), ToDouble(transformedZ.im), wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

void SinoidalRenderer::Render()
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

