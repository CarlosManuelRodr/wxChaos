#include "RenderWorker.h"
#include "FractalUtilities.h"
#include <algorithm>
#include <cmath>

RenderWorker::RenderWorker()
{
    _setMap = nullptr;
    _colorMap = nullptr;
    _auxMap = nullptr;
    _x = _y = 0;
    _threadProgress = 0;
    _widthOrigin = _heightOrigin = _widthFinal = _heightFinal = _oldHeightOrigin = 0;

    _stopped = false;
    _threadRunning = false;

    _type = FractalType::Undefined;
    _xFactor = 0.0;
    _yFactor = 0.0;
    _minX = _maxX = _minY = _maxY = _maxIter = 0.0;
    _preciseXFactor = 0;
    _preciseYFactor = 0;
    _useHighPrecision = false;
    _highPrecisionBits = 0;
    _renderingPrecisionMode = RenderingPrecisionMode::Adaptative;
    _kReal = _kImaginary = 0.0;
}

wxString RenderWorker::GetAlgorithmName(const RenderingAlgorithmType algorithm)
{
    switch (algorithm)
    {
        case RenderingAlgorithmType::EscapeTime: return "escape time";
        case RenderingAlgorithmType::GaussianInt: return "Gaussian integer";
        case RenderingAlgorithmType::EscapeAngle: return "escape angle";
        case RenderingAlgorithmType::TriangleInequality: return "triangle inequality";
        case RenderingAlgorithmType::ChaoticMap: return "chaotic map";
        case RenderingAlgorithmType::Lyapunov: return "Lyapunov";
        case RenderingAlgorithmType::ConvergenceTest: return "convergence test";
        case RenderingAlgorithmType::Buddhabrot: return "Buddhabrot";
        default: return "renderer-specific";
    }
}

void RenderWorker::SetOptions(const Options& opt)
{
    _myOpt = opt;
    _xFactor = opt.xFactor;
    _yFactor = opt.yFactor;
    _minX = opt.minX;
    _maxX = opt.maxX;
    _minY = opt.minY;
    _maxY = opt.maxY;
    _maxIter = opt.maxIter;
    _preciseView = opt.hasPreciseView ? opt.preciseView : PreciseRect(Rect(opt.minX, opt.minY, opt.maxX, opt.maxY));
    _preciseXFactor = opt.preciseXFactor;
    _preciseYFactor = opt.preciseYFactor;
    _useHighPrecision = opt.useHighPrecision;
    _highPrecisionBits = opt.highPrecisionBits;
    _renderingPrecisionMode = opt.renderingPrecisionMode;
    _type = opt.type;
}
void RenderWorker::SetLimits(const int widthOrigin, const int heightOrigin, const int widthFinal, const int heightFinal)
{
    _widthOrigin = widthOrigin;
    _oldHeightOrigin = _heightOrigin = heightOrigin;
    _heightFinal = heightFinal;
    _widthFinal = widthFinal;
}
void RenderWorker::UpdateLimits(const int heightOrigin)
{
    _heightOrigin = heightOrigin;
}
void RenderWorker::SetOldHeightOrigin(const int oldHeightOrigin)
{
    _oldHeightOrigin = oldHeightOrigin;
}
void RenderWorker::run()
{
    _y = _heightOrigin;
    _threadRunning = true;
    _stopped = false;

    this->Render();

    _threadRunning = false;
}
void RenderWorker::Stop()
{
    if (_type != FractalType::ScriptFractal)
    {
        _stopped = true;

        if (_y != 0)
            _heightOrigin = _y;

        _x = _widthFinal - 1;
        _y = _heightFinal - 1;
    }
}
void RenderWorker::SetRenderOut(bool** outSetMap, double** outColorMap, unsigned int** outAux)
{
    _setMap = outSetMap;
    _colorMap = outColorMap;
    _auxMap = outAux;
}
void RenderWorker::SetK(const double re, const double im)
{
    _kReal = re;
    _kImaginary = im;
}
double RenderWorker::ToColorMapValue(const double value)
{
    return !std::isfinite(value) || value < 0.0 ? InvalidColor : value;
}
double RenderWorker::SafeDistance(const double distance)
{
    return distance == 0.0 ? 0.000001 : distance;
}
double RenderWorker::InitialMu()
{
    return 1.0;
}
double RenderWorker::MuFromNorm(const double norm)
{
    return (log(log(2.0)) - log(log(sqrt(norm)))) / log(2.0) + 1;
}
void RenderWorker::MeasureEscapeMu(Point& point, const PointTraceEvent event, const double zNorm)
{
    if (event == PointTraceEvent::Escaped)
        point.mu = MuFromNorm(zNorm);
}
void RenderWorker::MeasureOrbitTrap(Point& point, const PointTraceEvent event, const double zRe, const double zIm)
{
    if (event == PointTraceEvent::Started)
    {
        point.orbitTrapDistanceX = abs(zRe);
        point.orbitTrapDistanceY = abs(zIm);
        return;
    }

    if (event == PointTraceEvent::Iterated)
    {
        point.orbitTrapDistanceX = minVal(point.orbitTrapDistanceX, abs(zRe));
        point.orbitTrapDistanceY = minVal(point.orbitTrapDistanceY, abs(zIm));
    }
}
void RenderWorker::MeasureGaussianInteger(Point& point, const PointTraceEvent event, const double zRe, const double zIm, const bool wasInside)
{
    if (event == PointTraceEvent::Escaped)
    {
        point.measureGaussianAfterEscape = true;
        return;
    }

    if (event != PointTraceEvent::Iterated || (!wasInside && !point.measureGaussianAfterEscape))
        return;

    point.previousGaussianDistance = point.gaussianDistance;
    point.gaussianDistance = minVal(point.gaussianDistance, gaussianIntDist(zRe, zIm));
    if (!wasInside)
        point.measureGaussianAfterEscape = false;
}
void RenderWorker::MeasureTriangleInequality(Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
                                         const double squaredRe, const double squaredIm, const bool wasInside)
{
    if (event != PointTraceEvent::Iterated || !wasInside)
        return;

    point.previousTriangleDistance = point.triangleDistance;
    if (iteration > 0)
    {
        point.triangleDistance += TIA(zRe, zIm, point.startRe, point.startIm, squaredRe, squaredIm);
        point.triangleIterations++;
    }
}
double RenderWorker::SmoothEscapeValue(const Point& point)
{
    return point.iterations + 1.0 - log(log(sqrt(point.escapedNorm))) / log(2.0);
}
double RenderWorker::OrbitTrapValue(const Point& point)
{
    return log(1 / SafeDistance(point.orbitTrapDistanceX)) + log(1 / SafeDistance(point.orbitTrapDistanceY));
}
double RenderWorker::EscapeTimeColor(const Point& point) const
{
    if (_myOpt.orbitTrapMode)
    {
        const double orbitTrapValue = OrbitTrapValue(point);
        const double escapeValue = _myOpt.smoothRender && !point.insideSet
                                       ? SmoothEscapeValue(point)
                                       : static_cast<double>(point.iterations);
        return ToColorMapValue(std::max(0.0, escapeValue + orbitTrapValue));
    }

    if (_myOpt.smoothRender && !point.insideSet)
        return ToColorMapValue(std::max(0.0, SmoothEscapeValue(point)));

    return point.iterations;
}
double RenderWorker::GaussianIntegerColor(const Point& point) const
{
    const double gaussianValue = (point.mu * point.gaussianDistance + (1 - point.mu) * point.previousGaussianDistance) * _myOpt.paletteSize;
    const double orbitTrapValue = _myOpt.orbitTrapMode ? OrbitTrapValue(point) : 0.0;
    return ToColorMapValue(std::max(0.0, gaussianValue + orbitTrapValue));
}
double RenderWorker::EscapeAngleColor(const Point& point) const
{
    constexpr int color1 = 1;
    const int color2 = static_cast<int>(0.25 * _myOpt.paletteSize);
    const int color3 = static_cast<int>(0.50 * _myOpt.paletteSize);
    const int color4 = static_cast<int>(0.75 * _myOpt.paletteSize);

    if (point.zRe > 0 && point.zIm > 0)
        return point.iterations + color1;
    if (point.zRe <= 0 && point.zIm > 0)
        return point.iterations + color2;
    if (point.zRe <= 0 && point.zIm < 0)
        return point.iterations + color3;
    return point.iterations + color4;
}
// ReSharper disable once CppMemberFunctionMayBeStatic
double RenderWorker::TriangleInequalityColor(const Point& point) const // NOLINT(*-convert-member-functions-to-static)
{
    if (point.triangleIterations <= 1)
        return 0;

    const double previousDistance = point.previousTriangleDistance / (point.triangleIterations - 1);
    const double distance = point.triangleDistance / point.triangleIterations;
    return ToColorMapValue(std::abs((point.mu * distance + (1 - point.mu) * previousDistance) * 700));
}
void RenderWorker::Reset()
{
    _x = 0;
    _y = 0;
}
void RenderWorker::PreTerminate()
{
    // Do nothing.
}
unsigned int RenderWorker::GetProgress()
{
    if (!_stopped)
        _threadProgress = static_cast<int>(floor(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin))));

    return _threadProgress;
}
Vector2Int RenderWorker::GetCoords() const
{
    const Vector2Int pos{0, _heightOrigin};
    return pos;
}
Vector2Int RenderWorker::GetStartPoints() const
{
    const Vector2Int pos{_widthOrigin, _heightOrigin};
    return pos;
}
Vector2Int RenderWorker::GetEndPoints() const
{
    const Vector2Int pos{_widthFinal, _heightFinal};
    return pos;
}
bool RenderWorker::IsRunning() const
{
    return _threadRunning;
}
Options RenderWorker::GetOptions()
{
    return _myOpt;
}
