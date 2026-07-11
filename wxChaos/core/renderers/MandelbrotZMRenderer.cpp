#include "MandelbrotZMRenderer.h"
#include <cmath>
#include <limits>
using namespace std;

MandelbrotZMRenderer::MandelbrotZMRenderer()
{
    _m = 0.0;
    _integerM = 0;
    _useIntegerExponent = true;
    _bailout = 0;
}

bool MandelbrotZMRenderer::IsIntegerExponent(const double exponent, int& integerExponent)
{
    if (!std::isfinite(exponent) || exponent < static_cast<double>(std::numeric_limits<int>::min()) ||
        exponent > static_cast<double>(std::numeric_limits<int>::max()))
        return false;

    const double roundedExponent = std::round(exponent);
    if (std::abs(exponent - roundedExponent) > 1e-12)
        return false;

    integerExponent = static_cast<int>(roundedExponent);
    return true;
}

template<class Real>
PrecisionComplex<Real> MandelbrotZMRenderer::Power(const PrecisionComplex<Real>& z) const
{
    if (_useIntegerExponent)
        return ComplexPow(z, _integerM);

    return ComplexPow(z, _m);
}

template<class Real, class MeasurePoint>
RenderWorker::Point MandelbrotZMRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> c(pixelRe, pixelIm);
    PrecisionComplex<Real> z(Real(0), Real(0));
    const Real squaredBail = Real(_bailout) * Real(_bailout);
    const Real trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIterations; i++)
    {
        const PrecisionComplex<Real> poweredZ = Power(z);
        z = poweredZ + c;

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

void MandelbrotZMRenderer::Render()
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

void MandelbrotZMRenderer::SetParams(const double m, const double bailout)
{
    _m = m;
    _useIntegerExponent = IsIntegerExponent(_m, _integerM);
    _bailout = bailout;
}
