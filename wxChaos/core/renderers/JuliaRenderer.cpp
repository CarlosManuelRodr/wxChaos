// ReSharper disable CppTooWideScope
#include "JuliaRenderer.h"
using namespace std;

JuliaRenderer::JuliaRenderer() = default;

template<class MeasurePoint>
Renderer::Point JuliaRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.zRe = pixelRe;
    point.zIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    double zRe = pixelRe;
    double zIm = pixelIm;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zNorm = zRe * zRe + zIm * zIm;
        if (!escaped && point.zNorm > 4)
        {
            const double zNorm = point.zNorm;
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = zRe;
            point.escapedZIm = zIm;
            point.escapedNorm = zNorm;
            measure(point, PointTraceEvent::Escaped, n, zRe, zIm, zNorm, 0.0, 0.0, true);
        }

        if (escaped && point.zNorm > 16 && !point.measureGaussianAfterEscape)
            break;

        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double squaredRe = zRe2 - zIm2;
        const double squaredIm = 2.0 * zRe * zIm;

        zRe = squaredRe + _kReal;
        zIm = squaredIm + _kImaginary;
        const double zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = zRe;
        point.zIm = zIm;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, zRe, zIm, zNorm, squaredRe, squaredIm, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

template<class MeasurePoint>
void JuliaRenderer::RenderFromPoint(unsigned int (JuliaRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    RenderPixels([this, colorPoint, measure](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void JuliaRenderer::EscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(&JuliaRenderer::EscapeTimeColor, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&JuliaRenderer::EscapeTimeColor, measure);
}

void JuliaRenderer::GaussianIntRender()
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
        RenderFromPoint(&JuliaRenderer::GaussianIntegerColor, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&JuliaRenderer::GaussianIntegerColor, measure);
}

void JuliaRenderer::EscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&JuliaRenderer::EscapeAngleColor, measure);
}

void JuliaRenderer::TriangleInequalityRender()
{
    const auto measure = [](Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
                            const double zNorm, const double squaredRe, const double squaredIm, const bool wasInside)
    {
        MeasureTriangleInequality(point, event, iteration, zRe, zIm, squaredRe, squaredIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&JuliaRenderer::TriangleInequalityColor, measure);
}

void JuliaRenderer::Render()
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
        case RenderingAlgorithmType::TriangleInequality:
            TriangleInequalityRender();
            break;
        default:
            break;
    }
}
