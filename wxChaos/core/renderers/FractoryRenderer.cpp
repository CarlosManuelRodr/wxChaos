// ReSharper disable CppTooWideScope
#include <complex>
#include "FractoryRenderer.h"

using namespace std;

FractoryRenderer::FractoryRenderer() = default;

template<class MeasurePoint>
Renderer::Point FractoryRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.zRe = pixelRe;
    point.zIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    const complex<double> c(pixelRe, pixelIm);
    complex<double> z = c;
    complex<double> b = c - sin(c);
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zNorm = norm(z);
        if (!escaped && point.zNorm > 4)
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = z.real();
            point.escapedZIm = z.imag();
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && !point.measureGaussianAfterEscape)
            break;

        b = c + b / c - z;
        z = z * c + b / z;
        const bool wasInside = !escaped;

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = norm(z);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);

        if (!escaped)
            point.iterations = n + 1;
    }

    return point;
}

void FractoryRenderer::Render()
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
