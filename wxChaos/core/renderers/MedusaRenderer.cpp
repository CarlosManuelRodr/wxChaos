#include <complex>
#include "MedusaRenderer.h"
#include "FractalUtils.h"
using namespace std;

MedusaRenderer::MedusaRenderer() = default;

void MedusaRenderer::EscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(&MedusaRenderer::ColorEscapeTimePoint, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&MedusaRenderer::ColorEscapeTimePoint, measure);
}

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

        if (escaped && point.zNorm > 16)
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

template<class MeasurePoint>
void MedusaRenderer::RenderFromPoint(unsigned int (MedusaRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    RenderPixels([this, colorPoint, measure](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void MedusaRenderer::GaussianIntRender()
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
        RenderFromPoint(&MedusaRenderer::ColorGaussianIntegerPoint, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&MedusaRenderer::ColorGaussianIntegerPoint, measure);
}

void MedusaRenderer::EscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&MedusaRenderer::ColorEscapeAnglePoint, measure);
}

unsigned int MedusaRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int MedusaRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int MedusaRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
}

void MedusaRenderer::Render()
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
