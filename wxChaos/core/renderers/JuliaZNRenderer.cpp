// ReSharper disable CppTooWideScope
#include <complex>
#include "JuliaZNRenderer.h"
#include "FractalUtils.h"
using namespace std;

JuliaZNRenderer::JuliaZNRenderer()
{
    _n = 0;
    _bailout = 0;
}
void JuliaZNRenderer::EscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(&JuliaZNRenderer::ColorEscapeTimePoint, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&JuliaZNRenderer::ColorEscapeTimePoint, measure);
}

template<class MeasurePoint>
Renderer::Point JuliaZNRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    const double squaredBail = _bailout * _bailout;
    const double trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        z = pow(z, _n) + k;

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

        if (escaped && point.zNorm > trapBailout)
            break;
    }

    return point;
}

template<class MeasurePoint>
void JuliaZNRenderer::RenderFromPoint(unsigned int (JuliaZNRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    RenderPixels([this, colorPoint, measure](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void JuliaZNRenderer::GaussianIntRender()
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
        RenderFromPoint(&JuliaZNRenderer::ColorGaussianIntegerPoint, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&JuliaZNRenderer::ColorGaussianIntegerPoint, measure);
}

void JuliaZNRenderer::EscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&JuliaZNRenderer::ColorEscapeAnglePoint, measure);
}

unsigned int JuliaZNRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int JuliaZNRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int JuliaZNRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    Point colorPoint = point;
    colorPoint.iterations = _n;
    return EscapeAngleColor(colorPoint);
}
void JuliaZNRenderer::Render()
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
void JuliaZNRenderer::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

