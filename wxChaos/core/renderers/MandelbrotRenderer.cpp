// ReSharper disable CppDFAConstantConditions
#include <complex>
#include <random>
#include <vector>
#include "MandelbrotRenderer.h"

MandelbrotRenderer::MandelbrotRenderer()
{
    _buddhaRandomP = 0;
}

template<class Real, class MeasurePoint>
Renderer::Point MandelbrotRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    Real zRe = 0;
    Real zIm = 0;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real zRe2 = zRe * zRe;
        const Real zIm2 = zIm * zIm;
        const Real squaredRe = zRe2 - zIm2;
        const Real squaredIm = Real(2) * zRe * zIm;

        zRe = squaredRe + pixelRe;
        zIm = squaredIm + pixelIm;
        const Real zNorm = zRe * zRe + zIm * zIm;
        const bool wasInside = !escaped;

        point.zRe = ToDouble(zRe);
        point.zIm = ToDouble(zIm);
        point.zNorm = ToDouble(zNorm);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(squaredRe), ToDouble(squaredIm), wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;

            if (zNorm > Real(4))
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = n;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, n, point.zRe, point.zIm, point.zNorm,
                        ToDouble(squaredRe), ToDouble(squaredIm), wasInside);
            }
        }

        if (escaped && zNorm > Real(16) && !point.measureGaussianAfterEscape)
            break;
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
    const auto tracePoint = [this](const auto& pixelRe, const auto& pixelIm, auto measure)
    {
        return TracePoint(pixelRe, pixelIm, measure);
    };

    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender(tracePoint);
            break;
        case RenderingAlgorithmType::GaussianInt:
            GaussianIntRender(tracePoint);
            break;
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender(tracePoint);
            break;
        case RenderingAlgorithmType::TriangleInequality:
            TriangleInequalityRender(tracePoint);
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

