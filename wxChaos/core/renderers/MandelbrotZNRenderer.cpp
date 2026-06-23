#include "MandelbrotZNRenderer.h"
using namespace std;

MandelbrotZNRenderer::MandelbrotZNRenderer()
{
    _n = 0;
    _bailout = 0;
}

template<class Real, class MeasurePoint>
Renderer::Point MandelbrotZNRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> c(pixelRe, pixelIm);
    PrecisionComplex<Real> z(Real(0), Real(0));
    const Real squaredBail = Real(_bailout) * Real(_bailout);
    const Real trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        const PrecisionComplex<Real> poweredZ = ComplexPow(z, _n);
        z = poweredZ + c;

        const Real zNorm = ComplexNorm(z);
        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(zNorm);
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, i, point.zRe, point.zIm, point.zNorm,
                ToDouble(poweredZ.re), ToDouble(poweredZ.im), wasInside);

        if (!escaped)
        {
            point.iterations = i + 1;

            if (zNorm > squaredBail)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = i;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, i, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);
            }
        }

        if (escaped && zNorm > trapBailout && !point.measureGaussianAfterEscape)
            break;
    }

    return point;
}

MandelbrotZNRenderer::PerturbationReference MandelbrotZNRenderer::BuildPerturbationReference(const HighPrecisionReal& centerRe,
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

    const PrecisionComplex<HighPrecisionReal> c{reference.centerRe, reference.centerIm};
    PrecisionComplex<HighPrecisionReal> z{HighPrecisionReal(0), HighPrecisionReal(0)};
    for (size_t i = 0; i + 1 < orbitSize; i++)
    {
        z = ComplexPow(z, _n) + c;
        reference.orbitRe[i + 1] = ToDouble(z.re);
        reference.orbitIm[i + 1] = ToDouble(z.im);
    }

    return reference;
}

template<class MeasurePoint>
MandelbrotZNRenderer::PerturbationTraceResult MandelbrotZNRenderer::TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
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
    const double squaredBail = _bailout * _bailout;
    const double trapBailout = squaredBail * squaredBail;
    bool escaped = false;

    for (unsigned i = 0; i < _maxIter; i++)
    {
        const PrecisionComplex<double> referenceZ{reference.orbitRe[i], reference.orbitIm[i]};
        const PrecisionComplex<double> epsilon{epsilonRe, epsilonIm};
        const PrecisionComplex<double> z = referenceZ + epsilon;
        const PrecisionComplex<double> poweredZ = ComplexPow(z, _n);
        const PrecisionComplex<double> poweredReferenceZ = ComplexPow(referenceZ, _n);

        epsilonRe = poweredZ.re - poweredReferenceZ.re + deltaRe;
        epsilonIm = poweredZ.im - poweredReferenceZ.im + deltaIm;

        const double nextZRe = reference.orbitRe[i + 1] + epsilonRe;
        const double nextZIm = reference.orbitIm[i + 1] + epsilonIm;
        const double zNorm = nextZRe * nextZRe + nextZIm * nextZIm;
        const double referenceNextNorm = reference.orbitRe[i + 1] * reference.orbitRe[i + 1] +
                                         reference.orbitIm[i + 1] * reference.orbitIm[i + 1];
        if (HasPerturbationGlitchOrDiverged(referenceNextNorm, zNorm, escaped, squaredBail, trapBailout))
            return {point, false};

        point.zRe = nextZRe;
        point.zIm = nextZIm;
        point.zNorm = zNorm;
        const bool wasInside = !escaped;
        measure(point, PointTraceEvent::Iterated, i, point.zRe, point.zIm, point.zNorm,
                poweredZ.re, poweredZ.im, wasInside);

        if (!escaped)
        {
            point.iterations = i + 1;

            if (zNorm > squaredBail)
            {
                escaped = true;
                point.insideSet = false;
                point.iterations = i;
                point.escapedZRe = point.zRe;
                point.escapedZIm = point.zIm;
                point.escapedNorm = point.zNorm;
                measure(point, PointTraceEvent::Escaped, i, point.zRe, point.zIm, point.zNorm, 0.0, 0.0, wasInside);
            }
        }

        if (escaped && zNorm > trapBailout && !point.measureGaussianAfterEscape)
            break;
    }

    return {point, true};
}

void MandelbrotZNRenderer::Render()
{
    if (ShouldUsePerturbationRender())
    {
        const auto renderFromPoint = [this](auto colorPoint, auto measure)
        {
            const auto buildReference = [this](const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm)
            {
                return BuildPerturbationReference(pixelRe, pixelIm);
            };
            const auto tracePerturbation = [this](const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                  const PerturbationReference& reference, auto measure)
            {
                return TracePerturbationPoint(pixelRe, pixelIm, reference, measure);
            };
            const auto tracePrecise = [this](const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm, auto measure)
            {
                return TracePoint(pixelRe, pixelIm, measure);
            };
            PerturbationRenderFromPoint(buildReference, tracePerturbation, tracePrecise, colorPoint, measure);
        };

        switch (_myOpt.alg)
        {
            case RenderingAlgorithmType::EscapeTime:
                PerturbationEscapeTimeRender(renderFromPoint);
                break;
            case RenderingAlgorithmType::GaussianInt:
                PerturbationGaussianIntRender(renderFromPoint);
                break;
            case RenderingAlgorithmType::EscapeAngle:
                PerturbationEscapeAngleRender(renderFromPoint);
                break;
            case RenderingAlgorithmType::TriangleInequality:
                PerturbationTriangleInequalityRender(renderFromPoint);
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
        default:
            break;
    }
}

void MandelbrotZNRenderer::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

