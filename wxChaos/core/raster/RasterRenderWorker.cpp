#include "RasterRenderWorker.h"

#include "../FractalUtilities.h"
#include <algorithm>

RasterRenderWorker::RasterRenderWorker()
{
    _setMap = nullptr;
    _colorMap = nullptr;
    _x = _y = 0;
    _threadProgress = 0;
    _widthOrigin = _heightOrigin = _widthFinal = _heightFinal = _oldHeightOrigin = 0;
    _stopped = false;

    _type = FractalType::Undefined;
    _xFactor = 0.0;
    _yFactor = 0.0;
    _minX = _maxX = _minY = _maxY = _maxIterations = 0.0;
    _preciseXFactor = 0;
    _preciseYFactor = 0;
    _useHighPrecision = false;
    _highPrecisionBits = 0;
    _renderingPrecisionMode = RenderingPrecisionMode::Adaptative;
    _kReal = _kImaginary = 0.0;
}

void RasterRenderWorker::SetOptions(const Options& opt)
{
    _myOpt = opt;
    _xFactor = opt.xFactor;
    _yFactor = opt.yFactor;
    _minX = opt.minX;
    _maxX = opt.maxX;
    _minY = opt.minY;
    _maxY = opt.maxY;
    _maxIterations = opt.maxIterations;
    _useHighPrecision = opt.useHighPrecision;
    _highPrecisionBits = opt.highPrecisionBits;
    if (_useHighPrecision)
    {
        _preciseView = opt.hasPreciseView ? opt.preciseView : PreciseRect(Rect(opt.minX, opt.minY, opt.maxX, opt.maxY));
        _preciseXFactor = opt.preciseXFactor;
        _preciseYFactor = opt.preciseYFactor;
    }
    _renderingPrecisionMode = opt.renderingPrecisionMode;
    _type = opt.type;
}
void RasterRenderWorker::SetLimits(const int widthOrigin, const int heightOrigin, const int widthFinal, const int heightFinal)
{
    _widthOrigin = widthOrigin;
    _oldHeightOrigin = _heightOrigin = heightOrigin;
    _heightFinal = heightFinal;
    _widthFinal = widthFinal;
}
void RasterRenderWorker::UpdateLimits(const int heightOrigin)
{
    _heightOrigin = heightOrigin;
}
void RasterRenderWorker::SetOldHeightOrigin(const int oldHeightOrigin)
{
    _oldHeightOrigin = oldHeightOrigin;
}
void RasterRenderWorker::Start()
{
    _y = _heightOrigin;
    _stopped = false;

    this->Render();
}
void RasterRenderWorker::Stop()
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
void RasterRenderWorker::SetOutputRenderingMaps(bool** outSetMap, double** outColorMap)
{
    _setMap = outSetMap;
    _colorMap = outColorMap;
}
void RasterRenderWorker::SetK(const double re, const double im)
{
    _kReal = re;
    _kImaginary = im;
}
double RasterRenderWorker::ToColorMapValue(const double value)
{
    return !std::isfinite(value) || value < 0.0 ? InvalidColor : value;
}
double RasterRenderWorker::SafeDistance(const double distance)
{
    return distance == 0.0 ? 0.000001 : distance;
}
double RasterRenderWorker::InitialMu()
{
    return 1.0;
}
double RasterRenderWorker::MuFromNorm(const double norm)
{
    return (log(log(2.0)) - log(log(sqrt(norm)))) / log(2.0) + 1;
}
void RasterRenderWorker::MeasureEscapeMu(Point& point, const PointTraceEvent event, const double zNorm)
{
    if (event == PointTraceEvent::Escaped)
        point.mu = MuFromNorm(zNorm);
}
void RasterRenderWorker::MeasureOrbitTrap(Point& point, const PointTraceEvent event, const double zRe, const double zIm)
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
void RasterRenderWorker::MeasureGaussianInteger(Point& point, const PointTraceEvent event, const double zRe, const double zIm, const bool wasInside)
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
void RasterRenderWorker::MeasureTriangleInequality(Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
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
double RasterRenderWorker::SmoothEscapeValue(const Point& point)
{
    return point.iterations + 1.0 - log(log(sqrt(point.escapedNorm))) / log(2.0);
}
double RasterRenderWorker::OrbitTrapValue(const Point& point)
{
    return log(1 / SafeDistance(point.orbitTrapDistanceX)) + log(1 / SafeDistance(point.orbitTrapDistanceY));
}
double RasterRenderWorker::EscapeTimeColor(const Point& point) const
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
double RasterRenderWorker::GaussianIntegerColor(const Point& point) const
{
    const double gaussianValue = (point.mu * point.gaussianDistance + (1 - point.mu) * point.previousGaussianDistance) * _myOpt.paletteSize;
    const double orbitTrapValue = _myOpt.orbitTrapMode ? OrbitTrapValue(point) : 0.0;
    return ToColorMapValue(std::max(0.0, gaussianValue + orbitTrapValue));
}
double RasterRenderWorker::EscapeAngleColor(const Point& point) const
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
double RasterRenderWorker::TriangleInequalityColor(const Point& point) const // NOLINT(*-convert-member-functions-to-static)
{
    if (point.triangleIterations <= 1)
        return 0;

    const double previousDistance = point.previousTriangleDistance / (point.triangleIterations - 1);
    const double distance = point.triangleDistance / point.triangleIterations;
    return ToColorMapValue(std::abs((point.mu * distance + (1 - point.mu) * previousDistance) * 700));
}
void RasterRenderWorker::Reset()
{
    _x = 0;
    _y = 0;
}
void RasterRenderWorker::PreTerminate()
{
    // Do nothing.
}
unsigned int RasterRenderWorker::GetProgress() const
{
    if (!_stopped)
        _threadProgress = static_cast<int>(floor(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin))));

    return _threadProgress;
}
Vector2Int RasterRenderWorker::GetCoords() const
{
    const Vector2Int pos{0, _heightOrigin};
    return pos;
}
Vector2Int RasterRenderWorker::GetStartPoints() const
{
    const Vector2Int pos{_widthOrigin, _heightOrigin};
    return pos;
}
Vector2Int RasterRenderWorker::GetEndPoints() const
{
    const Vector2Int pos{_widthFinal, _heightFinal};
    return pos;
}
Options RasterRenderWorker::GetOptions()
{
    return _myOpt;
}
