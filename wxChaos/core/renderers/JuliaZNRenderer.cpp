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
    RenderFromPoint(&JuliaZNRenderer::ColorEscapeTimePoint);
}

Renderer::Point JuliaZNRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

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

void JuliaZNRenderer::RenderFromPoint(unsigned int (JuliaZNRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void JuliaZNRenderer::GaussianIntRender()
{
    RenderFromPoint(&JuliaZNRenderer::ColorGaussianIntegerPoint);
}

void JuliaZNRenderer::EscapeAngleRender()
{
    RenderFromPoint(&JuliaZNRenderer::ColorEscapeAnglePoint);
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

