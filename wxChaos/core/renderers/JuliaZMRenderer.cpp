// ReSharper disable CppTooWideScope
#include "JuliaZMRenderer.h"
using namespace std;

JuliaZMRenderer::JuliaZMRenderer()
{
    _m = 0;
    _bailout = 0;
}

template<class Real, class MeasurePoint>
RenderWorker::Point JuliaZMRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> k{Real(_kReal), Real(_kImaginary)};
    PrecisionComplex<Real> z(pixelRe, pixelIm);
    const Real squaredBail = Real(_bailout) * Real(_bailout);
    const Real trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        const PrecisionComplex<Real> poweredZ = ComplexPow(z, _m);
        z = poweredZ + k;

        const Real zNorm = ComplexNorm(z);
        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(zNorm);
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, i, point.zRe, point.zIm, point.zNorm,
                ToDouble(poweredZ.re), ToDouble(poweredZ.im), wasInside);

        if (!escaped)
        {
            point.iterations = i + 1;

            if (zNorm > squaredBail)
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

        if (escaped && zNorm > trapBailout && !point.measureGaussianAfterEscape)
            break;
    }

    return point;
}

void JuliaZMRenderer::Render()
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
void JuliaZMRenderer::SetParams(const int n, const double bailout)
{
    _m = n;
    _bailout = bailout;
}

