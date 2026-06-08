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
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint MandelbrotZNRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    complex<double> c(pixelRe, pixelIm);
    complex<double> z = c;
    const double squaredBail = _bailout * _bailout;

    if (!_myOpt.orbitTrapMode)
    {
        unsigned i = 0;
        for (; i < _maxIter; i++)
        {
            z = pow(z, _n) + c;
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
        z = pow(z, _n) + c;
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

unsigned int MandelbrotZNRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode)
    {
        if (_myOpt.smoothRender)
            return ToColorMapValue(abs(4.0 * (point.iterations - SmoothEscapeOffset(point.zNorm))));

        return point.iterations;
    }

    const double trapOffset = OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY);
    if (!_myOpt.smoothRender)
        return ToColorMapValue(abs(point.iterations + trapOffset));

    if (!point.insideSet)
        return ToColorMapValue(abs(4.0 * (point.iterations - SmoothEscapeOffset(point.zNorm)) + 4.0 * trapOffset));

    return ToColorMapValue(abs(4.0 * (point.iterations + 4.0 * trapOffset)));
}

void MandelbrotZNRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint MandelbrotZNRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    complex<double> z(0, 0);
    const complex<double> c(pixelRe, pixelIm);
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
            point.insideSet = false;
        }

        z = pow(z, _n) + c;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(z.real(), z.imag()));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(z.imag()));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(z.real()));
    }

    return point;
}

void MandelbrotZNRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint MandelbrotZNRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    const complex<double> c(pixelRe, pixelIm);
    complex<double> z = c;
    const double squaredBail = _bailout * _bailout;

    unsigned i = 0;
    for (; i < _maxIter; i++)
    {
        z = pow(z, _n) + c;
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

