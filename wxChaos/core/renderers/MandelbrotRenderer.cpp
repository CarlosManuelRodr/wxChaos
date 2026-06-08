// ReSharper disable CppDFAConstantConditions
// ReSharper disable CppTooWideScope
#include <complex>
#include <random>
#include <vector>
#include "MandelbrotRenderer.h"
#include "FractalUtils.h"

MandelbrotRenderer::MandelbrotRenderer()
{
    _buddhaRandomP = 0;
}
void MandelbrotRenderer::EscapeTimeRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = ColorEscapePoint(point);
    });
}

Renderer::EscapePoint MandelbrotRenderer::IterateEscapePoint(const double pixelRe, const double pixelIm) const
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

            zIm = 2 * zRe * zIm + pixelIm;
            zRe = zRe2 - zIm2 + pixelRe;
        }

        point.iterations = n;
        point.zRe = zRe;
        point.zIm = zIm;
        return point;
    }

    point.trapDistanceX = abs(zRe);
    point.trapDistanceY = abs(zIm);

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        point.zNorm = zRe2 + zIm2;

        if (point.zNorm > 4)
        {
            point.insideSet = false;
            if (point.zNorm > 16)
            {
                point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
                point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));
                break;
            }
        }
        else
            point.iterations = n;

        zIm = 2 * zRe * zIm + pixelIm;
        zRe = zRe2 - zIm2 + pixelRe;

        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));
    }

    point.zRe = zRe;
    point.zIm = zIm;
    return point;
}

unsigned int MandelbrotRenderer::ColorEscapePoint(const EscapePoint& point) const
{
    if (!_myOpt.orbitTrapMode)
    {
        if (_myOpt.smoothRender)
            return static_cast<unsigned int>(abs(4.0 * (point.iterations - SmoothEscapeOffset(point.zNorm))));

        return point.iterations;
    }

    const double trapOffset = OrbitTrapColorOffset(point.trapDistanceX, point.trapDistanceY);
    if (!_myOpt.smoothRender)
        return ToColorMapValue(point.iterations + trapOffset);

    if (!point.insideSet)
        return ToColorMapValue(abs(4.0 * (point.iterations - SmoothEscapeOffset(point.zNorm)) + 4.0 * trapOffset));

    return ToColorMapValue(abs(4.0 * (point.iterations + 4.0 * trapOffset)));
}

void MandelbrotRenderer::GaussianIntRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const GaussianIntegerPoint point = IterateGaussianIntegerPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = GaussianIntegerColor(point, _myOpt.paletteSize);
    });
}

Renderer::GaussianIntegerPoint MandelbrotRenderer::IterateGaussianIntegerPoint(const double pixelRe, const double pixelIm) const
{
    GaussianIntegerPoint point;
    double zRe = 0.0;
    double zIm = 0.0;
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

        zIm = 2 * zRe * zIm + pixelIm;
        zRe = zRe2 - zIm2 + pixelRe;

        point.previousDistance = point.distance;
        point.distance = minVal(point.distance, gaussianIntDist(zRe, zIm));
        point.trapDistanceY = minVal(point.trapDistanceY, abs(zIm));
        point.trapDistanceX = minVal(point.trapDistanceX, abs(zRe));
    }

    return point;
}

void MandelbrotRenderer::EscapeAngleRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const EscapePoint point = IterateEscapeAnglePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = EscapeAngleColor(point, _myOpt.paletteSize);
    });
}

Renderer::EscapePoint MandelbrotRenderer::IterateEscapeAnglePoint(const double pixelRe, const double pixelIm) const
{
    EscapePoint point;
    double zRe = pixelRe;
    double zIm = pixelIm;

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

        zIm = 2 * zRe * zIm + pixelIm;
        zRe = zRe2 - zIm2 + pixelRe;
    }

    point.iterations = n;
    point.zRe = zRe;
    point.zIm = zIm;
    return point;
}

void MandelbrotRenderer::TriangleInequalityRender()
{
    RenderPixels([this](const double pixelRe, const double pixelIm)
    {
        const TriangleInequalityPoint point = IterateTriangleInequalityPoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = TriangleInequalityColor(point);
    });
}

Renderer::TriangleInequalityPoint MandelbrotRenderer::IterateTriangleInequalityPoint(const double pixelRe, const double pixelIm) const
{
    TriangleInequalityPoint point;
    double zRe = 0.0;
    double zIm = 0.0;
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

        zRe += pixelRe;
        zIm += pixelIm;

        point.previousDistance = point.distance;
        if (n > 0)
            point.distance += TIA(zRe, zIm, pixelRe, pixelIm, tiaPrevX, tiaPrevY);

        point.iterations = n + 1;
    }

    return point;
}

void MandelbrotRenderer::BuddhabrotRender()
{
    sf::Mutex mutex;
    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice());
    std::uniform_real_distribution<> randomX(_minX, _maxX);
    std::uniform_real_distribution<> randomY(_minY, _maxY);

    std::complex<double> c;
    std::vector<std::complex<double>> cmpArray(static_cast<unsigned int>(_maxIter));
    int topIter = 0;

    for (int i=0; i<_maxIter; i++)
        cmpArray[i] = std::complex<double>(0, 0);

    for (int bd=0; bd<_buddhaRandomP && !_stopped; bd++)
    {
        if (bd % 1000 == 0)
            _threadProgress = static_cast<unsigned int>(100.0 * (static_cast<double>(bd) / static_cast<double>(_buddhaRandomP)));

        bool out = false;

        std::complex<double> z = c = std::complex<double>(randomX(randomEngine), randomY(randomEngine));
        if
        (
           (z.real() >  -1.2 && z.real() <=  -1.1 && z.imag() >  -0.1 && z.imag() < 0.1)
            || (z.real() >  -1.1 && z.real() <=  -0.9 && z.imag() >  -0.2 && z.imag() < 0.2)
            || (z.real() >  -0.9 && z.real() <=  -0.8 && z.imag() >  -0.1 && z.imag() < 0.1)
            || (z.real() > -0.69 && z.real() <= -0.61 && z.imag() >  -0.2 && z.imag() < 0.2)
            || (z.real() > -0.61 && z.real() <=  -0.5 && z.imag() > -0.37 && z.imag() < 0.37)
            || (z.real() >  -0.5 && z.real() <= -0.39 && z.imag() > -0.48 && z.imag() < 0.48)
            || (z.real() > -0.39 && z.real() <=  0.14 && z.imag() > -0.55 && z.imag() < 0.55)
            || (z.real() >  0.14 && z.real() <   0.29 && z.imag() > -0.42 && z.imag() < -0.07)
            || (z.real() >  0.14 && z.real() <   0.29 && z.imag() >  0.07 && z.imag() < 0.42)
        ) continue; // "if" taken from Wikipedia description.

        for (int i=0; i<_maxIter && !_stopped; i++)
        {
            if (z.real()*z.real() + z.imag()*z.imag() > 6)
            {
                out = true;
                topIter = i;
                break;
            }
            cmpArray[i] = z;
            z = pow(z, 2) + c;
        }
        if (out && !_stopped)
        {
            for (int i=0; i<=topIter && !_stopped; i++)
            {
                const int indexI = static_cast<int>((cmpArray[i].real()-_minX)/_xFactor);
                const int indexJ = static_cast<int>((_maxY-cmpArray[i].imag())/_yFactor);
                if ((indexI >= 0 && indexI < _myOpt.screenWidth) && (indexJ >=0 && indexJ < _myOpt.screenHeight))
                {
                    mutex.lock();
                    _colorMap[indexI][indexJ]++;
                    mutex.unlock();
                }
            }

            // Takes advantage of the simmetry.
            z = c = std::complex<double>(c.real(), -c.imag());
            for (int i=0; i<_maxIter && !_stopped; i++)
            {
                z = pow(z,2) + c;
                const int indexI = static_cast<int>((z.real()-_minX)/_xFactor);
                const int indexJ = static_cast<int>((_maxY-z.imag())/_yFactor);
                if ((indexI >= 0 && indexI < _myOpt.screenWidth) && (indexJ >=0 && indexJ < _myOpt.screenHeight))
                {
                    mutex.lock();
                    _colorMap[indexI][indexJ]++;
                    mutex.unlock();
                }
            }
        }
    }

    if (!_stopped)
        _threadProgress = 100;
}

void MandelbrotRenderer::Render()
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
        case RenderingAlgorithmType::Buddhabrot:
            BuddhabrotRender();
            break;
        default:
            break;
    }
}
void MandelbrotRenderer::SetBuddhaRandomP(const int n)
{
    _buddhaRandomP = n;
}
unsigned int MandelbrotRenderer::GetProgress()
{
    if (_myOpt.alg == RenderingAlgorithmType::Buddhabrot)
        return _threadProgress;

    if (!_stopped)
    {
        _threadProgress = static_cast<int>(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin)));
    }
    return _threadProgress;
}

