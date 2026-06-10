#include <complex>
#include "MandelbrotZNRenderer.h"
#include "FractalUtils.h"
using namespace std;

MandelbrotZNRenderer::MandelbrotZNRenderer()
{
    _n = 0;
    _bailout = 0;
}

template<class MeasurePoint>
Renderer::Point MandelbrotZNRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    const complex<double> c(pixelRe, pixelIm);
    complex<double> z(0, 0);
    const double squaredBail = _bailout * _bailout;
    const double trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        z = pow(z, _n) + c;

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, i, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);

        if (!escaped)
        {
            point.iterations = i + 1;

            if (point.zNorm > squaredBail)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = i;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, i, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);
            }
        }

        if (escaped && point.zNorm > trapBailout && !point.measureGaussianAfterEscape)
            break;
    }

    return point;
}

void MandelbrotZNRenderer::Render()
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

void MandelbrotZNRenderer::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

