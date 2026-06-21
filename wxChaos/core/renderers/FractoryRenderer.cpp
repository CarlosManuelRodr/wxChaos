// ReSharper disable CppTooWideScope
#include "FractoryRenderer.h"

using namespace std;

FractoryRenderer::FractoryRenderer() = default;

template<class Real, class MeasurePoint>
Renderer::Point FractoryRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    point.zRe = point.startRe;
    point.zIm = point.startIm;
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> c(pixelRe, pixelIm);
    PrecisionComplex<Real> z = c;
    PrecisionComplex<Real> b = c - ComplexSin(c);
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real currentNorm = ComplexNorm(z);
        point.zNorm = ToDouble(currentNorm);
        if (n > 0 && !escaped && currentNorm > Real(4))
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = ToDouble(z.re);
            point.escapedZIm = ToDouble(z.im);
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && !point.measureGaussianAfterEscape)
            break;

        b = c + b / c - z;
        z = z * c + b / z;
        const bool wasInside = !escaped;

        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(ComplexNorm(z));
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);

        if (!escaped)
            point.iterations = n + 1;
    }

    return point;
}

void FractoryRenderer::Render()
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
