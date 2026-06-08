#include <complex>
#include "MedusaRenderer.h"
#include "FractalUtils.h"
using namespace std;

MedusaRenderer::MedusaRenderer() = default;

void MedusaRenderer::EscapeTimeRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint MedusaRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);

    if (!_myOpt.orbitTrapMode)
    {
        unsigned n = 0;
        for (; n < _maxIter; n++)
        {
            point.zNorm = z.real() * z.real() + z.imag() * z.imag();
            if (point.zNorm > 4)
            {
                point.insideSet = false;
                break;
            }

            z = pow(z, 1.5) + k;
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
        z = pow(z, 1.5) + k;
        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;
        if (point.zNorm > 4)
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

unsigned int MedusaRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode)
        return point.iterations;

    return ToColorMapValue(point.iterations + OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY));
}

void MedusaRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint MedusaRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    point.mu = InitialGaussianMu();
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned n = 0; n < _maxIter && point.insideSet; n++)
    {
        const double zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (zNorm > 4)
        {
            point.mu = EscapedGaussianMu(zNorm);
            if (n > 0)
                point.insideSet = false;
        }

        z = pow(z, 1.5) + k;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(z.real(), z.imag()));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(z.imag()));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(z.real()));
    }

    return point;
}

void MedusaRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint MedusaRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);

    unsigned n = 0;
    for (; n < _maxIter; n++)
    {
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (point.zNorm > 4)
        {
            point.insideSet = false;
            break;
        }

        z = pow(z, 1.5) + k;
    }

    point.iterations = n;
    point.zRe = z.real();
    point.zIm = z.imag();
    return point;
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
