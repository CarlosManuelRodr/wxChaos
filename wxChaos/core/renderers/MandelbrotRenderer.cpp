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
    RenderFromPoint(&MandelbrotRenderer::ColorEscapeTimePoint);
}

Renderer::Point MandelbrotRenderer::TracePoint(const double pixelRe, const double pixelIm) const
{
    Point point;
    point.startRe = pixelRe;
    point.startIm = pixelIm;
    point.mu = InitialMu();
    point.orbitTrapDistanceX = abs(pixelRe);
    point.orbitTrapDistanceY = abs(pixelIm);

    double zRe = 0.0;
    double zIm = 0.0;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const double zRe2 = zRe * zRe;
        const double zIm2 = zIm * zIm;
        const double squaredRe = zRe2 - zIm2;
        const double squaredIm = 2.0 * zRe * zIm;

        zRe = squaredRe + pixelRe;
        zIm = squaredIm + pixelIm;

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

            if (point.zNorm > 4)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = n;
                point.escapedZRe = zRe;
                point.escapedZIm = zIm;
                point.escapedNorm = point.zNorm;
                point.mu = MuFromNorm(point.zNorm);
            }
        }

        if (escaped && point.zNorm > 16)
            break;
    }

    return point;
}

void MandelbrotRenderer::RenderFromPoint(unsigned int (MandelbrotRenderer::*colorPoint)(const Point&) const)
{
    RenderPixels([this, colorPoint](const double pixelRe, const double pixelIm)
    {
        const Point point = TracePoint(pixelRe, pixelIm);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    });
}

void MandelbrotRenderer::GaussianIntRender()
{
    RenderFromPoint(&MandelbrotRenderer::ColorGaussianIntegerPoint);
}

void MandelbrotRenderer::EscapeAngleRender()
{
    RenderFromPoint(&MandelbrotRenderer::ColorEscapeAnglePoint);
}

void MandelbrotRenderer::TriangleInequalityRender()
{
    RenderFromPoint(&MandelbrotRenderer::ColorTriangleInequalityPoint);
}

unsigned int MandelbrotRenderer::ColorEscapeTimePoint(const Point& point) const
{
    return EscapeTimeColor(point);
}

unsigned int MandelbrotRenderer::ColorGaussianIntegerPoint(const Point& point) const
{
    return GaussianIntegerColor(point);
}

unsigned int MandelbrotRenderer::ColorEscapeAnglePoint(const Point& point) const
{
    return EscapeAngleColor(point);
}

unsigned int MandelbrotRenderer::ColorTriangleInequalityPoint(const Point& point) const
{
    return TriangleInequalityColor(point);
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

