// ReSharper disable CppTooWideScope
#include "JuliaRenderer.h"
using namespace std;

JuliaRenderer::JuliaRenderer() = default;

template<class Real, class MeasurePoint>
RenderWorker::Point JuliaRenderer::TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const
{
    Point point;
    point.startRe = ToDouble(pixelRe);
    point.startIm = ToDouble(pixelIm);
    point.zRe = point.startRe;
    point.zIm = point.startIm;
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    const PrecisionComplex<Real> c{Real(_kReal), Real(_kImaginary)};
    PrecisionComplex<Real> z{pixelRe, pixelIm};
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const Real currentNorm = ComplexNorm(z);
        point.zNorm = ToDouble(currentNorm);
        if (n > 0 && !escaped && currentNorm > Real(4))
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = ToDouble(z.re);
            point.escapedZIm = ToDouble(z.im);
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && currentNorm > Real(16) && !point.measureGaussianAfterEscape)
            break;

        const PrecisionComplex<Real> squaredZ = z * z;
        z = squaredZ + c;
        const Real zNorm = ComplexNorm(z);
        const bool wasInside = !escaped;

        point.zRe = ToDouble(z.re);
        point.zIm = ToDouble(z.im);
        point.zNorm = ToDouble(zNorm);
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                ToDouble(squaredZ.re), ToDouble(squaredZ.im), wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return point;
}

JuliaRenderer::PerturbationReference JuliaRenderer::BuildPerturbationReference(const HighPrecisionReal& centerRe,
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
    reference.orbit.resize(orbitSize);

    const PrecisionComplex c{HighPrecisionReal(_kReal), HighPrecisionReal(_kImaginary)};
    PrecisionComplex z{reference.centerRe, reference.centerIm};
    reference.orbit[0] = {ToDouble(z.re), ToDouble(z.im)};
    for (size_t n = 0; n + 1 < orbitSize; n++)
    {
        z = z * z + c;
        reference.orbit[n + 1] = {ToDouble(z.re), ToDouble(z.im)};
    }

    return reference;
}

template<class MeasurePoint>
JuliaRenderer::PerturbationTraceResult JuliaRenderer::TracePerturbationPoint(const HighPrecisionReal& pixelRe, const HighPrecisionReal& pixelIm,
                                                                             const PerturbationReference& reference, MeasurePoint measure) const
{
    const double deltaRe = ToDouble(pixelRe - reference.centerRe);
    const double deltaIm = ToDouble(pixelIm - reference.centerIm);
    if (!std::isfinite(deltaRe) || !std::isfinite(deltaIm))
        return {Point{}, false};

    Point point;
    point.startRe = reference.centerReDouble + deltaRe;
    point.startIm = reference.centerImDouble + deltaIm;
    point.zRe = point.startRe;
    point.zIm = point.startIm;
    measure(point, PointTraceEvent::Started, 0, point.startRe, point.startIm, 0.0, 0.0, 0.0, true);

    PrecisionComplex<double> epsilon{deltaRe, deltaIm};
    bool escaped = false;

    for (unsigned n = 0; n < _maxIter; n++)
    {
        const PrecisionComplex<double> referenceZ = reference.orbit[n];
        const PrecisionComplex<double> z = referenceZ + epsilon;
        const double currentNorm = ComplexNorm(z);
        const double referenceNorm = ComplexNorm(referenceZ);
        if (n > 0 && HasPerturbationGlitchOrDiverged(referenceNorm, currentNorm, escaped))
            return {point, false};

        point.zNorm = currentNorm;
        if (n > 0 && !escaped && currentNorm > 4.0)
        {
            escaped = true;
            point.insideSet = false;
            point.iterations = n;
            point.escapedZRe = z.re;
            point.escapedZIm = z.im;
            point.escapedNorm = point.zNorm;
            measure(point, PointTraceEvent::Escaped, n, point.escapedZRe, point.escapedZIm, point.zNorm, 0.0, 0.0, true);
        }

        if (escaped && currentNorm > 16.0 && !point.measureGaussianAfterEscape)
            break;

        const PrecisionComplex<double> squaredZ = z * z;
        epsilon = 2.0 * referenceZ * epsilon + epsilon * epsilon;

        const PrecisionComplex<double> nextZ = reference.orbit[n + 1] + epsilon;
        const double zNorm = ComplexNorm(nextZ);
        const double referenceNextNorm = ComplexNorm(reference.orbit[n + 1]);
        if (HasPerturbationGlitchOrDiverged(referenceNextNorm, zNorm, escaped))
            return {point, false};

        const bool wasInside = !escaped;
        point.zRe = nextZ.re;
        point.zIm = nextZ.im;
        point.zNorm = zNorm;
        measure(point, PointTraceEvent::Iterated, n, point.zRe, point.zIm, point.zNorm,
                squaredZ.re, squaredZ.im, wasInside);

        if (!escaped)
        {
            point.iterations = n + 1;
        }
    }

    return {point, true};
}

void JuliaRenderer::Render()
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
