// ReSharper disable CppDFAConstantConditions
#include <complex>
#include <algorithm>
#include <cmath>
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

bool MandelbrotRenderer::ShouldUsePerturbationRender() const
{
    if (_myOpt.alg == RenderingAlgorithmType::Buddhabrot)
        return false;

    if (_renderingPrecisionMode == RenderingPrecisionMode::Fast)
        return true;

    return _renderingPrecisionMode == RenderingPrecisionMode::Adaptative && _useHighPrecision;
}

// ReSharper disable once CppDFAUnreachableFunctionCall
bool MandelbrotRenderer::HasPerturbationGlitchOrDiverged(const double referenceNorm, const double zNorm, const bool escaped)
{
    if (!std::isfinite(referenceNorm) || !std::isfinite(zNorm))
        return true;

    if (!escaped && zNorm <= 4.0 && referenceNorm > 16.0)
        return true;

    constexpr double glitchThreshold = 1.0e-6;
    return referenceNorm > 1.0e-12 && zNorm < referenceNorm * glitchThreshold;
}

MandelbrotRenderer::PerturbationReference MandelbrotRenderer::BuildPerturbationReference(const HighPrecisionReal& centerRe,
                                                                                         const HighPrecisionReal& centerIm) const
{
    PerturbationReference reference;
    const unsigned int precisionBits = std::max(_highPrecisionBits, 128U);
    HighPrecisionReal::PrecisionScope precision(precisionBits);

    reference.centerRe = HighPrecisionReal::WithCurrentPrecision(centerRe);
    reference.centerIm = HighPrecisionReal::WithCurrentPrecision(centerIm);
    reference.centerReDouble = ToDouble(reference.centerRe);
    reference.centerImDouble = ToDouble(reference.centerIm);

    const auto orbitSize = static_cast<size_t>(_maxIter) + 1;
    reference.orbitRe.resize(orbitSize, 0.0);
    reference.orbitIm.resize(orbitSize, 0.0);

    HighPrecisionReal zRe(0);
    HighPrecisionReal zIm(0);
    const HighPrecisionReal two(2);
    for (size_t n = 0; n + 1 < orbitSize; n++)
    {
        const HighPrecisionReal zRe2 = zRe * zRe;
        const HighPrecisionReal zIm2 = zIm * zIm;
        const HighPrecisionReal squaredRe = zRe2 - zIm2;
        const HighPrecisionReal squaredIm = two * zRe * zIm;

        zRe = squaredRe + reference.centerRe;
        zIm = squaredIm + reference.centerIm;

        reference.orbitRe[n + 1] = ToDouble(zRe);
        reference.orbitIm[n + 1] = ToDouble(zIm);
    }

    return reference;
}

MandelbrotRenderer::PerturbationReference MandelbrotRenderer::BuildInitialPerturbationReference() const
{
    const unsigned int precisionBits = std::max(_highPrecisionBits, 128U);
    HighPrecisionReal::PrecisionScope precision(precisionBits);

    const HighPrecisionReal two(2);
    const HighPrecisionReal centerRe = (HighPrecisionReal::WithCurrentPrecision(_preciseView.left) +
                                        HighPrecisionReal::WithCurrentPrecision(_preciseView.right)) / two;
    const HighPrecisionReal centerIm = (HighPrecisionReal::WithCurrentPrecision(_preciseView.top) +
                                        HighPrecisionReal::WithCurrentPrecision(_preciseView.bottom)) / two;
    return BuildPerturbationReference(centerRe, centerIm);
}

template<class MeasurePoint>
MandelbrotRenderer::PerturbationTraceResult MandelbrotRenderer::TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                                                       const PerturbationReference& reference, MeasurePoint measure) const
{
    const double deltaRe = ToDouble(pixelRe - reference.centerRe);
    const double deltaIm = ToDouble(pixelIm - reference.centerIm);
    if (!std::isfinite(deltaRe) || !std::isfinite(deltaIm))
        return {Point{}, false};

    Point point;
    point.startRe = reference.centerReDouble + deltaRe;
    point.startIm = reference.centerImDouble + deltaIm;
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    double epsilonRe = 0.0;
    double epsilonIm = 0.0;
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const double referenceRe = reference.orbitRe[n];
        const double referenceIm = reference.orbitIm[n];
        const double zRe = referenceRe + epsilonRe;
        const double zIm = referenceIm + epsilonIm;
        const double squaredRe = zRe * zRe - zIm * zIm;
        const double squaredIm = 2.0 * zRe * zIm;

        const double epsilonSquaredRe = epsilonRe * epsilonRe - epsilonIm * epsilonIm;
        const double epsilonSquaredIm = 2.0 * epsilonRe * epsilonIm;
        const double referenceEpsilonRe = 2.0 * (referenceRe * epsilonRe - referenceIm * epsilonIm);
        const double referenceEpsilonIm = 2.0 * (referenceRe * epsilonIm + referenceIm * epsilonRe);

        epsilonRe = referenceEpsilonRe + epsilonSquaredRe + deltaRe;
        epsilonIm = referenceEpsilonIm + epsilonSquaredIm + deltaIm;

        const double nextZRe = reference.orbitRe[n + 1] + epsilonRe;
        const double nextZIm = reference.orbitIm[n + 1] + epsilonIm;
        const double zNorm = nextZRe * nextZRe + nextZIm * nextZIm;
        const double referenceNextNorm = reference.orbitRe[n + 1] * reference.orbitRe[n + 1] +
                                         reference.orbitIm[n + 1] * reference.orbitIm[n + 1];
        if (HasPerturbationGlitchOrDiverged(referenceNextNorm, zNorm, escaped))
            return {point, false};

        const bool wasInside = !escaped;
        point.zRe = nextZRe;
        point.zIm = nextZIm;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                squaredRe, squaredIm, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;

            if (zNorm > 4.0)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = n;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, n, point.zRe, point.zIm, point.zNorm,
                        squaredRe, squaredIm, wasInside);
            }
        }

        if (escaped && zNorm > 16.0 && !point.measureGaussianAfterEscape)
            break;
    }

    return {point, true};
}

template<class PixelRenderer>
void MandelbrotRenderer::RenderPerturbationPixels(PixelRenderer pixelRenderer)
{
    const unsigned int precisionBits = std::max(_highPrecisionBits, 128U);
    HighPrecisionReal::PrecisionScope precision(precisionBits);
    const HighPrecisionReal top = HighPrecisionReal::WithCurrentPrecision(_preciseView.top);
    const HighPrecisionReal left = HighPrecisionReal::WithCurrentPrecision(_preciseView.left);
    const HighPrecisionReal xFactor = HighPrecisionReal::WithCurrentPrecision(_preciseXFactor);
    const HighPrecisionReal yFactor = HighPrecisionReal::WithCurrentPrecision(_preciseYFactor);
    HighPrecisionReal pixelIm = top - HighPrecisionReal(_heightOrigin) * yFactor;

    for (_y = _heightOrigin; _y < _heightFinal; _y++)
    {
        HighPrecisionReal pixelRe = left + HighPrecisionReal(_widthOrigin) * xFactor;
        for (_x = _widthOrigin; _x < _widthFinal; _x++)
        {
            pixelRenderer(pixelRe, pixelIm);
            pixelRe += xFactor;
        }
        pixelIm -= yFactor;
    }
}

template<class MeasurePoint>
void MandelbrotRenderer::PerturbationRenderFromPoint(double (MandelbrotRenderer::*colorPoint)(const Point&) const, MeasurePoint measure)
{
    std::vector<PerturbationReference> references;
    references.push_back(BuildInitialPerturbationReference());

    const auto renderPixel = [this, colorPoint, measure, &references](const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm)
    {
        PerturbationTraceResult result = TracePerturbationPoint(pixelRe, pixelIm, references.back(), measure);
        if (!result.valid)
        {
            references.push_back(BuildPerturbationReference(pixelRe, pixelIm));
            result = TracePerturbationPoint(pixelRe, pixelIm, references.back(), measure);
        }

        const Point point = result.valid ? result.point : TracePoint(pixelRe, pixelIm, measure);
        if (point.insideSet)
            _setMap[_x][_y] = true;

        _colorMap[_x][_y] = (this->*colorPoint)(point);
    };

    RenderPerturbationPixels(renderPixel);
}

void MandelbrotRenderer::PerturbationEscapeTimeRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm, double, double, double, bool)
        {
            MeasureOrbitTrap(point, event, zRe, zIm);
        };
        PerturbationRenderFromPoint(&MandelbrotRenderer::EscapeTimeColor, measure);
        return;
    }

    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    PerturbationRenderFromPoint(&MandelbrotRenderer::EscapeTimeColor, measure);
}

void MandelbrotRenderer::PerturbationGaussianIntRender()
{
    if (_myOpt.orbitTrapMode)
    {
        const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                                const double zNorm, double, double, const bool wasInside)
        {
            MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
            MeasureOrbitTrap(point, event, zRe, zIm);
            MeasureEscapeMu(point, event, zNorm);
        };
        PerturbationRenderFromPoint(&MandelbrotRenderer::GaussianIntegerColor, measure);
        return;
    }

    const auto measure = [](Point& point, const PointTraceEvent event, unsigned int, const double zRe, const double zIm,
                            const double zNorm, double, double, const bool wasInside)
    {
        MeasureGaussianInteger(point, event, zRe, zIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    PerturbationRenderFromPoint(&MandelbrotRenderer::GaussianIntegerColor, measure);
}

void MandelbrotRenderer::PerturbationEscapeAngleRender()
{
    const auto measure = [](Point&, PointTraceEvent, unsigned int, double, double, double, double, double, bool) {};
    PerturbationRenderFromPoint(&MandelbrotRenderer::EscapeAngleColor, measure);
}

void MandelbrotRenderer::PerturbationTriangleInequalityRender()
{
    const auto measure = [](Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
                            const double zNorm, const double squaredRe, const double squaredIm, const bool wasInside)
    {
        MeasureTriangleInequality(point, event, iteration, zRe, zIm, squaredRe, squaredIm, wasInside);
        MeasureEscapeMu(point, event, zNorm);
    };
    PerturbationRenderFromPoint(&MandelbrotRenderer::TriangleInequalityColor, measure);
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
    if (ShouldUsePerturbationRender())
    {
        switch (_myOpt.alg)
        {
            case RenderingAlgorithmType::EscapeTime:
                PerturbationEscapeTimeRender();
                break;
            case RenderingAlgorithmType::GaussianInt:
                PerturbationGaussianIntRender();
                break;
            case RenderingAlgorithmType::EscapeAngle:
                PerturbationEscapeAngleRender();
                break;
            case RenderingAlgorithmType::TriangleInequality:
                PerturbationTriangleInequalityRender();
                break;
            default:
                break;
        }
        return;
    }

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

