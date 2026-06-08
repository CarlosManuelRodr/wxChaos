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
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint JuliaZNRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    const double squaredBail = _bailout * _bailout;

    if (!_myOpt.orbitTrapMode && !_myOpt.smoothRender)
    {
        unsigned i = 0;
        for (; i < _maxIter; i++)
        {
            z = pow(z, _n) + k;
            point.zNorm = z.real() * z.real() + z.imag() * z.imag();
            if (point.zNorm > squaredBail)
            {
                point.insideSet = false;
                break;
            }
        }

        point.iterations = i;
        point.zRe = z.real();
        point.zIm = z.imag();
        return point;
    }

    const double bailFourPower = squaredBail * squaredBail;
    point.trapDistanceX = abs(z.real());
    point.trapDistanceY = abs(z.imag());

    for (unsigned i = 0; i < _maxIter; i++)
    {
        z = pow(z, _n) + k;
        point.zRe = z.real();
        point.zIm = z.imag();
        point.zNorm = point.zRe * point.zRe + point.zIm * point.zIm;

        if (point.zNorm > squaredBail)
        {
            point.insideSet = false;
            if (point.zNorm > bailFourPower)
            {
                point.trapDistanceY = minVal(point.trapDistanceY, abs(point.zIm));
                point.trapDistanceX = minVal(point.trapDistanceX, abs(point.zRe));
                break;
            }
        }
        else
            point.iterations = i;

        point.trapDistanceY = minVal(point.trapDistanceY, abs(point.zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(point.zRe));
    }

    return point;
}

unsigned int JuliaZNRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode && !_myOpt.smoothRender)
        return point.iterations;

    const double trapOffset = OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY);
    if (!_myOpt.smoothRender)
        return ToColorMapValue(abs(point.iterations + trapOffset));

    if (!point.insideSet)
        return ToColorMapValue(abs(4.0 * (point.iterations - SmoothEscapeOffset(point.zNorm)) + 4.0 * trapOffset));

    return ToColorMapValue(abs(4.0 * (point.iterations + 4.0 * trapOffset)));
}

void JuliaZNRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint JuliaZNRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    const double squaredBail = _bailout * _bailout;
    point.mu = InitialGaussianMu();
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned i = 0; i < _maxIter && point.insideSet; i++)
    {
        const double zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (zNorm > squaredBail)
        {
            point.mu = EscapedGaussianMu(zNorm);
            if (i > 0)
                point.insideSet = false;
        }

        z = pow(z, _n) + k;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(z.real(), z.imag()));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(z.imag()));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(z.real()));
    }

    return point;
}

void JuliaZNRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        point.iterations = _n;
        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint JuliaZNRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const auto k = complex<double>(_kReal, _kImaginary);
    complex<double> z(pixelRe, pixelIm);
    const double squaredBail = _bailout * _bailout;

    unsigned i = 0;
    for (; i < _maxIter; i++)
    {
        z = pow(z, _n) + k;
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (point.zNorm > squaredBail)
        {
            point.insideSet = false;
            break;
        }
    }

    point.iterations = i;
    point.zRe = z.real();
    point.zIm = z.imag();
    return point;
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

