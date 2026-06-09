#include <complex>
#include "MedusaRenderer.h"
#include "FractalUtils.h"
using namespace std;

MedusaRenderer::MedusaRenderer() = default;

void MedusaRenderer::EscapeTimeRender()
{
    RenderFromPoint(&MedusaRenderer::ColorEscapeTimePoint);
}

Renderer::Point MedusaRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

    const complex<double> constant(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (!escaped && point.zNorm > 4)
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = z.real();
            point.escapedZIm = z.imag();
            point.escapedNorm = point.zNorm;
            point.mu = MuFromNorm(point.zNorm);
        }

        if (escaped && point.zNorm > 16)
            break;

        z = pow(z, 1.5) + constant;

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        point.orbitTrapDistanceX = minVal(point.orbitTrapDistanceX, abs(point.zRe));
        point.orbitTrapDistanceY = minVal(point.orbitTrapDistanceY, abs(point.zIm));

        if (!escaped)
        {
            point.iterations = n + 1;
            point.previousGaussianDistance = point.gaussianDistance;
            point.gaussianDistance = minVal(point.gaussianDistance, gaussianIntDist(point.zRe, point.zIm));
        }
    }

    return point;
}

void MedusaRenderer::RenderFromPoint(unsigned int (MedusaRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void MedusaRenderer::GaussianIntRender()
{
    RenderFromPoint(&MedusaRenderer::ColorGaussianIntegerPoint);
}

void MedusaRenderer::EscapeAngleRender()
{
    RenderFromPoint(&MedusaRenderer::ColorEscapeAnglePoint);
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
