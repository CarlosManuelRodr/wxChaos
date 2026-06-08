#include "ManowarRenderer.h"
#include "FractalUtils.h"

ManowarRenderer::ManowarRenderer() = default;

void ManowarRenderer::EscapeTimeRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint ManowarRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;
    double manRe = pixelRe;
    double manIm = pixelIm;

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

            const double tempRe = zRe;
            const double tempIm = zIm;
            zIm = 2 * zRe * zIm + pixelIm + manIm;
            zRe = zRe2 - zIm2 + pixelRe + manRe;
            manRe = tempRe;
            manIm = tempIm;
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
        point.zNorm = zRe2 + zIm2;
        if (point.zNorm > 4)
        {
            point.insideSet = false;
            broken = true;
        }

        const double tempRe = zRe;
        const double tempIm = zIm;
        zIm = 2 * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;

        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));

        if (!broken)
            point.iterations = n;
    }

    point.zRe = zRe;
    point.zIm = zIm;
    return point;
}

unsigned int ManowarRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode)
        return point.iterations;

    return ToColorMapValue(point.iterations + OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY));
}

void ManowarRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint ManowarRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;
    double manRe = pixelRe;
    double manIm = pixelIm;
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
            point.insideSet = false;
        }

        const double tempRe = zRe;
        const double tempIm = zIm;
        zIm = 2 * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(zRe, zIm));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));
    }

    return point;
}

void ManowarRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint ManowarRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;
    double manRe = pixelRe;
    double manIm = pixelIm;

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

        const double tempRe = zRe;
        const double tempIm = zIm;
        zIm = 2 * zRe * zIm + pixelIm + manIm;
        zRe = zRe2 - zIm2 + pixelRe + manRe;
        manRe = tempRe;
        manIm = tempIm;
    }

    point.iterations = n;
    point.zRe = zRe;
    point.zIm = zIm;
    return point;
}

void ManowarRenderer::Render()
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
