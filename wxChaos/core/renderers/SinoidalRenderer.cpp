// ReSharper disable CppTooWideScope
#include <complex>
#include "SinoidalRenderer.h"
using namespace std;

SinoidalRenderer::SinoidalRenderer() = default;

template<class MeasurePoint>
Renderer::Point SinoidalRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
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
        if (!escaped && point.zNorm > _maxIter)
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

        if (escaped && point.zNorm > _maxIter * _maxIter && !point.measureGaussianAfterEscape)
            break;

        z = constant * sin(z);

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

template<class MeasurePoint>
void SinoidalRenderer::RenderFromPoint(unsigned int (SinoidalRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    RenderPixels([this, colorPoint, measure](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void SinoidalRenderer::EscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(&SinoidalRenderer::EscapeTimeColor, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&SinoidalRenderer::EscapeTimeColor, measure);
}

void SinoidalRenderer::GaussianIntRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                                const double zNorm, double, double, const bool wasInside)
        {
            MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
            MeasureOrbitTrap(point, event, zRe, zIm);
            MeasureEscapeMu(point, event, zNorm);
        };
        RenderFromPoint(&SinoidalRenderer::GaussianIntegerColor, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&SinoidalRenderer::GaussianIntegerColor, measure);
}

void SinoidalRenderer::EscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&SinoidalRenderer::EscapeAngleColor, measure);
}

void SinoidalRenderer::Render()
{
    switch (_myOpt.alg)
    {
    case RenderingAlgorithmType::EscapeTime:
        EscapeTimeRender();
        break;
    case RenderingAlgorithmType::GaussianInt:
        GaussianIntRender();
        break;
    case RenderingAlgorithmType::EscapeAngle:
        EscapeAngleRender();
        break;
    default:
        break;
    }
}

