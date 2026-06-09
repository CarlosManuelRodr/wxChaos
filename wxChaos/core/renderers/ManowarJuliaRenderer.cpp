#include <complex>
#include "ManowarJuliaRenderer.h"
#include "FractalUtils.h"
using namespace std;

ManowarJuliaRenderer::ManowarJuliaRenderer() = default;

void ManowarJuliaRenderer::EscapeTimeRender()
{
    RenderFromPoint(&ManowarJuliaRenderer::ColorEscapeTimePoint);
}

Renderer::Point ManowarJuliaRenderer::TracePoint(const double pixelRe, const double pixelIm) const
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

        zIm = 2.0 * zRe * zIm + _kImaginary + manIm;
        zRe = zRe2 - zIm2 + _kReal + manRe;
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

void ManowarJuliaRenderer::RenderFromPoint(unsigned int (ManowarJuliaRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void ManowarJuliaRenderer::GaussianIntRender()
{
    RenderFromPoint(&ManowarJuliaRenderer::ColorGaussianIntegerPoint);
}

void ManowarJuliaRenderer::EscapeAngleRender()
{
    RenderFromPoint(&ManowarJuliaRenderer::ColorEscapeAnglePoint);
}

unsigned int ManowarJuliaRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int ManowarJuliaRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int ManowarJuliaRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
}

void ManowarJuliaRenderer::Render()
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

