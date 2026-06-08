// ReSharper disable CppTooWideScope
#include <complex>
#include "JuliaRenderer.h"
#include "FractalUtils.h"
using namespace std;

JuliaRenderer::JuliaRenderer() = default;

void JuliaRenderer::EscapeTimeRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint JuliaRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;

    if (!_myOpt.orbitTrapMode)
    {
        unsigned n = 0;
        for (; n < _maxIter; n++)
        {
            const double zRe2 = zRe * zRe;
            const double zIm2 = zIm * zIm;
            point.zNorm = zRe2 + zIm2;
            if (point.zNorm > 4)
            {
                point.insideSet = false;
                break;
            }

            zIm = 2 * zRe * zIm + _kImaginary;
            zRe = zRe2 - zIm2 + _kReal;
        }

        point.iterations = n;
        point.zRe = zRe;
        point.zIm = zIm;
        return point;
    }

    bool broken = false;
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        zIm = 2 * zRe * zIm + _kImaginary;
        zRe = zRe2 - zIm2 + _kReal;

        const double zNorm = zRe * zRe + zIm * zIm;
        if (zNorm > 4)
        {
            if (!broken)
                point.zNorm = zNorm;

            point.insideSet = false;
            broken = true;
        }

        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));

        if (!broken)
            point.iterations = n;
    }

    point.zRe = zRe;
    point.zIm = zIm;
    return point;
}

unsigned int JuliaRenderer::ColorEscapePoint(const EscapePoint& point) const
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

void JuliaRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint JuliaRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;
    point.mu = InitialGaussianMu();
    point.trapDistanceX = abs(pixelRe);
    point.trapDistanceY = abs(pixelIm);

    for (unsigned n = 0; n < _maxIter && point.insideSet; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double zNorm = zRe2 + zIm2;

        if (zNorm > 4)
        {
            point.mu = EscapedGaussianMu(zNorm);
            if (n > 0)
                point.insideSet = false;
        }

        zIm = 2 * zRe * zIm + _kImaginary;
        zRe = zRe2 - zIm2 + _kReal;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(zRe, zIm));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));
    }

    return point;
}

void JuliaRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint JuliaRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    complex<double> z(pixelRe, pixelIm);
    const complex<double> k(_kReal, _kImaginary);

    unsigned n = 0;
    for (; n < _maxIter; n++)
    {
        z = pow(z, 2) + k;
        point.zNorm = z.real() * z.real() + z.imag() * z.imag();
        if (point.zNorm > 4)
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

void JuliaRenderer::TriangleInequalityRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const TriangleInequalityPoint point = IterateTriangleInequalityPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = TriangleInequalityColor(point);
    });
}

Renderer::TriangleInequalityPoint JuliaRenderer::IterateTriangleInequalityPoint(const double pixelRe, const double pixelIm) const
{
    TriangleInequalityPoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;
    point.mu = InitialGaussianMu();

    for (unsigned n = 0; n < _maxIter && point.insideSet; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double zNorm = zRe2 + zIm2;

        if (zNorm > 4)
        {
            point.mu = EscapedGaussianMu(zNorm);
            if (n > 0)
                point.insideSet = false;
        }

        zIm = 2 * zRe * zIm;
        zRe = zRe2 - zIm2;

        const double tiaPrevX = zRe;
        const double tiaPrevY = zIm;

        zRe += _kReal;
        zIm += _kImaginary;

        point.previousDistance = point.distance;
        if (n > 0)
            point.distance += TIA(zRe, zIm, pixelRe, pixelIm, tiaPrevX, tiaPrevY);

        point.iterations = n + 1;
    }

    return point;
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
