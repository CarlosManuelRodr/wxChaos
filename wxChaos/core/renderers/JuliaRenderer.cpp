// ReSharper disable CppTooWideScope
#include <complex>
#include "JuliaRenderer.h"
#include "FractalUtils.h"
using namespace std;

JuliaRenderer::JuliaRenderer() = default;

void JuliaRenderer::EscapeTimeRender()
{
    RenderFromPoint(&JuliaRenderer::ColorEscapeTimePoint);
}

Renderer::Point JuliaRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.zRe = pixelRe;
    point.zIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

    double zRe = pixelRe;
    double zIm = pixelIm;
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
        const double squaredRe = zRe2 - zIm2;
        const double squaredIm = 2.0 * zRe * zIm;

        zRe = squaredRe + _kReal;
        zIm = squaredIm + _kImaginary;

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
            point.previousTriangleDistance = point.triangleDistance;
            if (n > 0)
            {
                point.triangleDistance += TIA(zRe, zIm, pixelRe, pixelIm, squaredRe, squaredIm);
                point.triangleIterations++;
            }
        }
    }

    return point;
}

void JuliaRenderer::RenderFromPoint(unsigned int (JuliaRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void JuliaRenderer::GaussianIntRender()
{
    RenderFromPoint(&JuliaRenderer::ColorGaussianIntegerPoint);
}

void JuliaRenderer::EscapeAngleRender()
{
    RenderFromPoint(&JuliaRenderer::ColorEscapeAnglePoint);
}

void JuliaRenderer::TriangleInequalityRender()
{
    RenderFromPoint(&JuliaRenderer::ColorTriangleInequalityPoint);
}

unsigned int JuliaRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int JuliaRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int JuliaRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
}

unsigned int JuliaRenderer::ColorTriangleInequalityPoint(const Point& point) const
{
    return TriangleInequalityColor(point);
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
