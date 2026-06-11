#include "Renderer.h"
#include "FractalUtils.h"

// RenderFractal
Renderer::Renderer() : _myOpt()
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
    _kReal = _kImaginary = 0.0;
}

void Renderer::SetOptions(const Options& opt)
{
    _myOpt = opt;
    _xFactor = opt.xFactor;
    _yFactor = opt.yFactor;
    _minX = opt.minX;
    _maxX = opt.maxX;
    _minY = opt.minY;
    _maxY = opt.maxY;
    _maxIter = opt.maxIter;
    _type = opt.type;
}
void Renderer::SetLimits(const int widthOrigin, const int heightOrigin, const int widthFinal, const int heightFinal)
{
    _widthOrigin = widthOrigin;
    _oldHeightOrigin = _heightOrigin = heightOrigin;
    _heightFinal = heightFinal;
    _widthFinal = widthFinal;
}
void Renderer::UpdateLimits(const int heightOrigin)
{
    _heightOrigin = heightOrigin;
}
void Renderer::SetOldHeightOrigin(const int oldHeightOrigin)
{
    _oldHeightOrigin = oldHeightOrigin;
}
void Renderer::run()
{
    _y = _heightOrigin;
    _threadRunning = true;
    _stopped = false;

    this->Render();

    _threadRunning = false;
}
void Renderer::Stop()
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
void Renderer::SetRenderOut(bool** outSetMap, unsigned int** outColorMap, unsigned int** outAux)
{
    _setMap = outSetMap;
    _colorMap = outColorMap;
    _auxMap = outAux;
}
void Renderer::SetK(const double re, const double im)
{
    _kReal = re;
    _kImaginary = im;
}
unsigned int Renderer::ToColorMapValue(const double value)
{
    return value < 0 ? InvalidColor : static_cast<unsigned int>(value);
}
double Renderer::SafeDistance(const double distance)
{
    return distance == 0.0 ? 0.000001 : distance;
}
double Renderer::InitialMu()
{
    return 1.0;
}
double Renderer::MuFromNorm(const double norm)
{
    return (log(log(2.0)) - log(log(sqrt(norm)))) / log(2.0) + 1;
}
void Renderer::MeasureEscapeMu(Point& point, const PointTraceEvent event, const double zNorm)
{
    if (event == PointTraceEvent::Escaped)
        point.mu = MuFromNorm(zNorm);
}
void Renderer::MeasureOrbitTrap(Point& point, const PointTraceEvent event, const double zRe, const double zIm)
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
void Renderer::MeasureGaussianInteger(Point& point, const PointTraceEvent event, const double zRe, const double zIm, const bool wasInside)
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
void Renderer::MeasureTriangleInequality(Point& point, const PointTraceEvent event, const unsigned int iteration, const double zRe, const double zIm,
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
double Renderer::SmoothEscapeValue(const Point& point)
{
    return point.iterations - log(log(point.escapedNorm)) / log(2.0);
}
double Renderer::OrbitTrapValue(const Point& point)
{
    return log(1 / SafeDistance(point.orbitTrapDistanceX)) + log(1 / SafeDistance(point.orbitTrapDistanceY));
}
unsigned int Renderer::EscapeTimeColor(const Point& point) const
{
    if (_myOpt.orbitTrapMode)
    {
        const double orbitTrapValue = OrbitTrapValue(point);
        if (_myOpt.smoothRender)
        {
            if (!point.insideSet)
                return ToColorMapValue(abs(4.0 * SmoothEscapeValue(point) + 4.0 * orbitTrapValue));

            return ToColorMapValue(abs(4.0 * (point.iterations + 4.0 * orbitTrapValue)));
        }

        return ToColorMapValue(abs(point.iterations + orbitTrapValue));
    }

    if (_myOpt.smoothRender && !point.insideSet)
        return ToColorMapValue(abs(4.0 * SmoothEscapeValue(point)));

    return point.iterations;
}
unsigned int Renderer::GaussianIntegerColor(const Point& point) const
{
    const double gaussianValue = (point.mu * point.gaussianDistance + (1 - point.mu) * point.previousGaussianDistance) * _myOpt.paletteSize;
    const double orbitTrapValue = _myOpt.orbitTrapMode ? OrbitTrapValue(point) : 0.0;
    return ToColorMapValue(abs(gaussianValue + orbitTrapValue));
}
unsigned int Renderer::EscapeAngleColor(const Point& point) const
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
unsigned int Renderer::TriangleInequalityColor(const Point& point) const // NOLINT(*-convert-member-functions-to-static)
{
    if (point.triangleIterations <= 1)
        return 0;

    const double previousDistance = point.previousTriangleDistance / (point.triangleIterations - 1);
    const double distance = point.triangleDistance / point.triangleIterations;
    return static_cast<unsigned int>(abs(((point.mu * distance + (1 - point.mu) * previousDistance) * 700)));
}
void Renderer::Reset()
{
    _x = 0;
    _y = 0;
}
void Renderer::PreTerminate()
{
    // Do nothing.
}
unsigned int Renderer::GetProgress()
{
    if (!_stopped)
        _threadProgress = static_cast<int>(floor(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin))));

    return _threadProgress;
}
Vector2Int Renderer::GetCoords() const
{
    const Vector2Int pos{0, _heightOrigin};
    return pos;
}
Vector2Int Renderer::GetStartPoints() const
{
    const Vector2Int pos{_widthOrigin, _heightOrigin};
    return pos;
}
Vector2Int Renderer::GetEndPoints() const
{
    const Vector2Int pos{_widthFinal, _heightFinal};
    return pos;
}
bool Renderer::IsRunning() const
{
    return _threadRunning;
}
Options Renderer::GetOptions()
{
    return _myOpt;
}
