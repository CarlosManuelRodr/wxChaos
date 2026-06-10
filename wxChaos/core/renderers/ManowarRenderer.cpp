#include "ManowarRenderer.h"

ManowarRenderer::ManowarRenderer() = default;

template<class MeasurePoint>
Renderer::Point ManowarRenderer::TracePoint(const double pixelRe, const double pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    measure(point, PointTraceEvent::Started, 0, pixelRe, pixelIm, 0.0, 0.0, 0.0, true);

    double zRe = pixelRe;
    double zIm = pixelIm;
    double manRe = pixelRe;
    double manIm = pixelIm;
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
        const double tempRe = zRe;
        const double tempIm = zIm;

        zIm = 2 * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;
        const double zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = zRe;
        point.zIm = zIm;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, zRe, zIm, zNorm, zRe2 - zIm2, 2.0 * tempRe * tempIm, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

template<class MeasurePoint>
void ManowarRenderer::RenderFromPoint(unsigned int (ManowarRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    RenderPixels([this, colorPoint, measure](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void ManowarRenderer::EscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        RenderFromPoint(&ManowarRenderer::EscapeTimeColor, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&ManowarRenderer::EscapeTimeColor, measure);
}

void ManowarRenderer::GaussianIntRender()
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
        RenderFromPoint(&ManowarRenderer::GaussianIntegerColor, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    RenderFromPoint(&ManowarRenderer::GaussianIntegerColor, measure);
}

void ManowarRenderer::EscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    RenderFromPoint(&ManowarRenderer::EscapeAngleColor, measure);
}

void ManowarRenderer::Render()
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
