#include "ManowarRenderer.h"
#include "FractalUtils.h"

ManowarRenderer::ManowarRenderer() = default;

void ManowarRenderer::EscapeTimeRender()
{
    RenderFromPoint(&ManowarRenderer::ColorEscapeTimePoint);
}

Renderer::Point ManowarRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

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
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = zRe;
            point.escapedZIm = zIm;
            point.escapedNorm = point.zNorm;
            point.mu = MuFromNorm(point.zNorm);
        }

        if (escaped && point.zNorm > 16)
            break;

        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double tempRe = zRe;
        const double tempIm = zIm;

        zIm = 2 * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;

        point.zRe = zRe;
        point.zIm = zIm;
        point.zNorm = zRe * zRe + zIm * zIm;
        point.orbitTrapDistanceX = minVal(point.orbitTrapDistanceX, abs(zRe));
        point.orbitTrapDistanceY = minVal(point.orbitTrapDistanceY, abs(zIm));

        if (!escaped)
        {
            point.iterations = n + 1;
            point.previousGaussianDistance = point.gaussianDistance;
            point.gaussianDistance = minVal(point.gaussianDistance, gaussianIntDist(zRe, zIm));
        }
    }

    return point;
}

void ManowarRenderer::RenderFromPoint(unsigned int (ManowarRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void ManowarRenderer::GaussianIntRender()
{
    RenderFromPoint(&ManowarRenderer::ColorGaussianIntegerPoint);
}

void ManowarRenderer::EscapeAngleRender()
{
    RenderFromPoint(&ManowarRenderer::ColorEscapeAnglePoint);
}

unsigned int ManowarRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int ManowarRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int ManowarRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
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
