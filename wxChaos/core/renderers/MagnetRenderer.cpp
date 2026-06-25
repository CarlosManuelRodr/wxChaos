// ReSharper disable CppTooWideScope
#include "MagnetRenderer.h"

using namespace std;

MagnetRenderer::MagnetRenderer() = default;

template<class Real, class MeasurePoint>
RenderWorker::Point MagnetRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> c(pixelRe, pixelIm);
    PrecisionComplex<Real> z(Real(0), Real(0));
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real currentNorm = ComplexNorm(z);
        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(currentNorm);

        if (!escaped && currentNorm > Real(_maxIter))
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = point.zRe;
            point.escapedZIm = point.zIm;
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && !point.measureGaussianAfterEscape)
            break;

        z = ComplexPow((ComplexPow(z, 2) + c - PrecisionComplex<Real>(Real(1), Real(0))) /
                       (Real(2) * z + c - PrecisionComplex<Real>(Real(2), Real(0))), 2);

        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(ComplexNorm(z));
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);

        if (!escaped)
            point.iterations = n + 1;
    }

    return point;
}

void MagnetRenderer::Render()
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
