// ReSharper disable CppTooWideScope
#include <complex>
#include "SinoidalRenderer.h"
#include "FractalUtils.h"
using namespace std;

SinoidalRenderer::SinoidalRenderer() = default;

void SinoidalRenderer::EscapeTimeRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint SinoidalRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const complex<double> k(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);

    if (!_myOpt.orbitTrapMode)
    {
        unsigned n = 0;
        for (; n < _maxIter; n++)
        {
            point.zNorm = z.real() * z.real() + z.imag() * z.imag();
            if (point.zNorm > _maxIter)
            {
                point.insideSet = false;
                break;
            }

            z = k * sin(z);
        }

        point.iterations = n;
        point.zRe = z.real();
        point.zIm = z.imag();
        return point;
    }

    bool broken = false;
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned n = 0; n < _maxIter; n++)
    {
        z = k * sin(z);
        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        if (point.zNorm > _maxIter)
        {
            point.insideSet = false;
            broken = true;
        }

        point.trapDistanceY = minVal(point.trapDistanceY, abs(point.zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(point.zRe));

        if (!broken)
            point.iterations = n;
    }

    return point;
}

unsigned int SinoidalRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode)
        return point.iterations;

    return ToColorMapValue(abs(point.iterations + OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY)));
}

void SinoidalRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint SinoidalRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    const complex<double> k(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    point.mu = InitialGaussianMu();
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned n = 0; n < _maxIter && point.insideSet; n++)
    {
        const double zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (zNorm > _maxIter)
        {
            point.mu = EscapedGaussianMu(zNorm);
            if (n > 0)
                point.insideSet = false;
        }

        z = k * sin(z);

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(z.real(), z.imag()));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(z.imag()));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(z.real()));
    }

    return point;
}

void SinoidalRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint SinoidalRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const complex<double> k(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);

    unsigned n = 0;
    for (; n < _maxIter; n++)
    {
        z = k * sin(z);
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (point.zNorm > _maxIter)
        {
            point.insideSet = false;
            break;
        }
    }

    point.iterations = n;
    point.zRe = z.real();
    point.zIm = z.imag();
    return point;
}

void SinoidalRenderer::Render()
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

