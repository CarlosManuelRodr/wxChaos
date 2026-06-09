#include <complex>
#include "MandelbrotZNRenderer.h"
#include "FractalUtils.h"
using namespace std;

MandelbrotZNRenderer::MandelbrotZNRenderer()
{
    _n = 0;
    _bailout = 0;
}
void MandelbrotZNRenderer::EscapeTimeRender()
{
    RenderFromPoint(&MandelbrotZNRenderer::ColorEscapeTimePoint);
}

Renderer::Point MandelbrotZNRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

    const complex<double> c(pixelRe, pixelIm);
    complex<double> z(0, 0);
    const double squaredBail = _bailout * _bailout;
    const double trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        z = pow(z, _n) + c;

        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        point.orbitTrapDistanceX = minVal(point.orbitTrapDistanceX, abs(point.zRe));
        point.orbitTrapDistanceY = minVal(point.orbitTrapDistanceY, abs(point.zIm));

        if (!escaped)
        {
            point.iterations = i + 1;
            point.previousGaussianDistance = point.gaussianDistance;
            point.gaussianDistance = minVal(point.gaussianDistance, gaussianIntDist(point.zRe, point.zIm));

            if (point.zNorm > squaredBail)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = i;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                point.mu = MuFromNorm(point.zNorm);
            }
        }

        if (escaped && point.zNorm > trapBailout)
            break;
    }

    return point;
}

void MandelbrotZNRenderer::RenderFromPoint(unsigned int (MandelbrotZNRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void MandelbrotZNRenderer::GaussianIntRender()
{
    RenderFromPoint(&MandelbrotZNRenderer::ColorGaussianIntegerPoint);
}

void MandelbrotZNRenderer::EscapeAngleRender()
{
    RenderFromPoint(&MandelbrotZNRenderer::ColorEscapeAnglePoint);
}

unsigned int MandelbrotZNRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int MandelbrotZNRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int MandelbrotZNRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
}

void MandelbrotZNRenderer::Render()
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

void MandelbrotZNRenderer::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

