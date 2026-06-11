// ReSharper disable CppTooWideScope
#include <complex>
#include "MagnetRenderer.h"

using namespace std;

MagnetRenderer::MagnetRenderer() = default;

template<class MeasurePoint>
Renderer::Point MagnetRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    const complex<double> c(pixelRe, pixelIm);
    complex<double> z(0.0, 0.0);

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = norm(z);

        if (point.zNorm > _maxIter)
        {
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = point.zRe;
            point.escapedZIm = point.zIm;
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, true);
            break;
        }

        z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0)) /
                (complex<double>(2.0, 0.0) * z + c - complex<double>(2.0, 0.0)), 2.0);

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = norm(z);
        point.iterations = n + 1;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, true);
    }

    return point;
}

void MagnetRenderer::Render()
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
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender(tracePoint);
            break;
        default:
            break;
    }
}
