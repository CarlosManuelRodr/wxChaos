#include <complex>
#include "MedusaRenderer.h"
using namespace std;

MedusaRenderer::MedusaRenderer() = default;

template<class MeasurePoint>
Renderer::Point MedusaRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    const complex<double> constant(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (!escaped && point.zNorm > 4)
        {
            const double zNorm = point.zNorm;
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = z.real();
            point.escapedZIm = z.imag();
            point.escapedNorm = zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, zNorm, 0.0, 0.0, true);
        }

        if (escaped && point.zNorm > 16 && !point.measureGaussianAfterEscape)
            break;

        z = pow(z, 1.5) + constant;

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

void MedusaRenderer::Render()
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
