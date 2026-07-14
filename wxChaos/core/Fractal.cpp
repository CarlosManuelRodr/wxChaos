#include <complex>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include "Fractal.h"
#include "FractalFactory.h"
#include "coloring/ColorPalette.h"
#include "coloring/RenderingAlgorithm.h"
#include "docs/FractalDocumentation.h"
using namespace std;

constexpr ColorPaletteTypes defaultGradientStyle = ClassicMandelbrot;

Fractal::Fractal(const unsigned int width, const unsigned int height)
{
    // Copy window properties.
    _screenHeight = height;
    _screenWidth = width;

    _fSetColor = wxColour(0, 0, 0);

    // Set fractal properties.
    _type = FractalType::None;
    _minX = -2.0;
    _maxX = 1.0;
    _minY = -1.2;
    _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);
    _preciseXFactor = 0;
    _preciseYFactor = 0;
    _preciseViewInitialized = false;
    _kReal = 0;
    _kImaginary = 0;
    _changeGradient = 0;
    _rendered = false;
    _colorRotation = false;
    _colorMode = true;
    _juliaMode = false;
    _hasOrbit = false;
    _orbitMode = false;
    _orbitDrawn = false;
    _onSnapshot = false;
    _juliaVariety = false;
    _colorSet = true;
    _orbitTrapMode = false;
    _hasOrbitTrap = false;
    _smoothRender = false;
    _hasSmoothRender = false;
    _hasHighPrecisionRender = false;
    _hasFastPrecisionRender = false;
    _waitRoutine = false;
    _redrawAll = false;
    _redrawAlways = false;
    _rendering = false;
    _paused = false;
    _pausing = false;
    _maxIterations = 100;
    _defaultIter = 100;
    _iterationStep = 100;
    _varGradChange = false;
    _refreshImage = false;
    _maxColorMapVal = 0.0;
    _relativeColorMin = 0.0;
    _relativeColorMax = 1.0;
    _colorCycleLength = 72.0;
    _colorRotationSpeed = 0.0;
    _colorRotationRemainder = 0.0;
    _paletteMappingMode = PaletteMappingMode::Linear;
    _paletteMappingExponent = 1.5;
    _orbitX = _orbitY = 0.0;
    _changeFractalProp = false;
    _reportedHighPrecisionActive = false;
    _reportedHighPrecisionBits = 0;
    _geomFigure = false;

    // Creates default color palette.
    ColorPalette defaultPalette;
    defaultPalette.SetStyle(defaultGradientStyle);

    _relativeColor = false;
    _gradPaletteSize = _paletteSize = defaultPalette.paletteSize;
    _algorithm = RenderingAlgorithmType::Other;
    _renderingPrecisionMode = RenderingPrecisionMode::Adaptative;
    _gradStyle = defaultGradientStyle;
    _gradient.FromString(wxString::FromUTF8(defaultPalette.grad.c_str()));
    _gradient.SetMin(0);
    _gradient.SetMax(_gradPaletteSize);
    _colorCycleLength = defaultPalette.colorCycleLength;

    _palette.resize(_paletteSize);
    _colorRotationSpeed = 120.0;
    for (int i = 0; i < _paletteSize; i++)
        _palette[i] = _gradient.GetColorAt(i);
}

Fractal::~Fractal() = default;

Fractal::CoordinateSystem Fractal::GetCoordinateSystem() const
{
    return {_("Real"), _("Imaginary")};
}

wxString Fractal::GetRenderingAlgorithmName() const
{
    switch (_algorithm)
    {
        case RenderingAlgorithmType::EscapeTime: return "escape time";
        case RenderingAlgorithmType::GaussianInt: return "Gaussian integer";
        case RenderingAlgorithmType::EscapeAngle: return "escape angle";
        case RenderingAlgorithmType::TriangleInequality: return "triangle inequality";
        case RenderingAlgorithmType::ChaoticMap: return "chaotic map";
        case RenderingAlgorithmType::ConvergenceTest: return "convergence test";
        case RenderingAlgorithmType::Buddhabrot: return "Buddhabrot";
        default: return "renderer-specific";
    }
}

sf::Color Fractal::InterpolatePaletteColors(const wxColour& first, const wxColour& second, const double ratio)
{
    const auto channel = [ratio](const unsigned char a, const unsigned char b)
    {
        return static_cast<sf::Uint8>(std::round((1.0 - ratio) * a + ratio * b));
    };

    return {channel(first.Red(), second.Red()),
            channel(first.Green(), second.Green()),
            channel(first.Blue(), second.Blue()),
            channel(first.Alpha(), second.Alpha())};
}

sf::Color Fractal::GetColorFromPalette(const double index) const
{
    if (_paletteSize == 0 || _palette.empty())
        return sf::Color::White;

    double wrappedIndex = std::fmod(index, static_cast<double>(_paletteSize));
    if (wrappedIndex < 0.0)
        wrappedIndex += _paletteSize;

    const auto lowerIndex = static_cast<unsigned int>(std::floor(wrappedIndex)) % _paletteSize;
    const auto upperIndex = (lowerIndex + 1) % _paletteSize;
    const double ratio = wrappedIndex - std::floor(wrappedIndex);
    return InterpolatePaletteColors(_palette[lowerIndex], _palette[upperIndex], ratio);
}
void Fractal::RebuildPalette()
{
    for (int i = 0; i < _paletteSize; i++)
    {
        const wxColour myWxColor = _gradient.GetColorAt(i);
        _palette[i] = myWxColor;
    }
    this->RedrawMaps();
}

// Color operations.
void Fractal::EnsurePreciseViewInitialized() const
{
    if (_preciseViewInitialized)
        return;

    _preciseView = PreciseRect(Rect(_minX, _minY, _maxX, _maxY));
    const HighPrecisionReal widthDivisor = _screenWidth > 1 ? HighPrecisionReal(_screenWidth - 1) : HighPrecisionReal(1);
    const HighPrecisionReal heightDivisor = _screenHeight > 1 ? HighPrecisionReal(_screenHeight - 1) : HighPrecisionReal(1);
    _preciseXFactor = (_preciseView.right - _preciseView.left) / widthDivisor;
    _preciseYFactor = (_preciseView.top - _preciseView.bottom) / heightDivisor;
    _preciseViewInitialized = true;
}
void Fractal::SyncDoubleViewFromPrecise()
{
    _minX = ToDouble(_preciseView.left);
    _maxX = ToDouble(_preciseView.right);
    _minY = ToDouble(_preciseView.bottom);
    _maxY = ToDouble(_preciseView.top);
    _xFactor = ToDouble(_preciseXFactor);
    _yFactor = ToDouble(_preciseYFactor);
}
void Fractal::UpdatePreciseFactors() const
{
    const HighPrecisionReal widthDivisor = _screenWidth > 1 ? HighPrecisionReal(_screenWidth - 1) : HighPrecisionReal(1);
    const HighPrecisionReal heightDivisor = _screenHeight > 1 ? HighPrecisionReal(_screenHeight - 1) : HighPrecisionReal(1);
    _preciseXFactor = (_preciseView.right - _preciseView.left) / widthDivisor;
    _preciseYFactor = (_preciseView.top - _preciseView.bottom) / heightDivisor;
    _preciseViewInitialized = true;
}
bool Fractal::ShouldUseHighPrecision() const
{
    if (!SupportsHighPrecisionRender())
        return false;

    if (_renderingPrecisionMode == RenderingPrecisionMode::Precise)
        return true;

    if (_renderingPrecisionMode == RenderingPrecisionMode::Fast)
        return false;

    return EstimateRequiredPrecisionBits() > 53;
}
bool Fractal::SupportsHighPrecisionRender() const
{
    if (_algorithm == RenderingAlgorithmType::Buddhabrot)
        return false;

    return _hasHighPrecisionRender;
}
bool Fractal::SupportsFastPrecisionRender() const
{
    return _hasFastPrecisionRender;
}
unsigned int Fractal::EstimateRequiredPrecisionBits() const
{
    EnsurePreciseViewInitialized();

    if (_screenWidth <= 1 || _screenHeight <= 1)
        return 0;

    using std::ceil;
    using std::log;
    using std::max;

    const auto antiAliasingScale = GetAntiAliasingScale();
    const auto renderWidth = std::max(1u, _screenWidth * antiAliasingScale);
    const auto renderHeight = std::max(1u, _screenHeight * antiAliasingScale);
    const HighPrecisionReal widthDivisor = antiAliasingScale > 1 || renderWidth <= 1
                                               ? HighPrecisionReal(renderWidth)
                                               : HighPrecisionReal(renderWidth - 1);
    const HighPrecisionReal heightDivisor = antiAliasingScale > 1 || renderHeight <= 1
                                                ? HighPrecisionReal(renderHeight)
                                                : HighPrecisionReal(renderHeight - 1);
    const HighPrecisionReal xFactor = (_preciseView.right - _preciseView.left) / widthDivisor;
    const HighPrecisionReal yFactor = (_preciseView.top - _preciseView.bottom) / heightDivisor;
    const HighPrecisionReal minStep = std::min(HighPrecisionReal(RealAbs(xFactor)),
                                               HighPrecisionReal(RealAbs(yFactor)));
    if (minStep <= 0)
        return 1024;

    const HighPrecisionReal coordinateScale = std::max(
        HighPrecisionReal(1),
        std::max(std::max(HighPrecisionReal(RealAbs(_preciseView.left)), HighPrecisionReal(RealAbs(_preciseView.right))),
                 std::max(HighPrecisionReal(RealAbs(_preciseView.bottom)), HighPrecisionReal(RealAbs(_preciseView.top)))));
    const HighPrecisionReal precisionRatio = coordinateScale / minStep;
    if (precisionRatio <= 0)
        return 0;

    const HighPrecisionReal log2Ratio = log(precisionRatio) / log(HighPrecisionReal(2));
    const double requiredBits = ceil(ToDouble(log2Ratio)) + 8.0;
    if (requiredBits <= 0.0)
        return 0;
    if (requiredBits >= static_cast<double>(std::numeric_limits<unsigned int>::max()))
        return std::numeric_limits<unsigned int>::max();

    return static_cast<unsigned int>(requiredBits);
}
void Fractal::NotifyPrecisionStatusIfChanged()
{
    const bool active = ShouldUseHighPrecision();
    const unsigned int precisionBits = active ? GetHighPrecisionRenderBits() : 0;
    if (active == _reportedHighPrecisionActive && precisionBits == _reportedHighPrecisionBits)
        return;

    _reportedHighPrecisionActive = active;
    _reportedHighPrecisionBits = precisionBits;
    if (_precisionStatusChanged)
        _precisionStatusChanged(active, precisionBits);
}
bool Fractal::OptionsPreciseViewMatchesDoubleView(const Options& opt)
{
    return opt.hasPreciseView &&
        ToDouble(opt.preciseView.left) == opt.minX &&
        ToDouble(opt.preciseView.right) == opt.maxX &&
        ToDouble(opt.preciseView.bottom) == opt.minY &&
        ToDouble(opt.preciseView.top) == opt.maxY;
}
void Fractal::ConfigureIterationDefaults(const unsigned int defaultIterations, const unsigned int iterationStep)
{
    _defaultIter = std::max(1U, defaultIterations);
    _iterationStep = std::max(1U, iterationStep);
    _maxIterations = _defaultIter;
}
void Fractal::SetView(const Rect& worldCoordinates)
{
    SetPreciseView(PreciseRect(worldCoordinates));
}

void Fractal::SetPreciseView(const PreciseRect& worldCoordinates)
{
    _preciseView = worldCoordinates;
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();
    _rendered = false;
    _rendering = false;

}
void Fractal::Redraw()
{
    this->StopRender();
    _redrawAll = true;
    _rendered = false;
    _rendering = false;
    _paused = false;
}

sf::Vector2u Fractal::GetScreenSize() const
{
    return {_screenWidth, _screenHeight};
}

Rect Fractal::GetView() const
{
    return GetPreciseView().ToRect();
}

PreciseRect Fractal::GetPreciseView() const
{
    EnsurePreciseViewInitialized();
    return _preciseView;
}

Rect Fractal::GetViewForPixelRect(const sf::Rect<int>& pixelCoordinates) const
{
    return GetPreciseViewForPixelRect(pixelCoordinates).ToRect();
}

PreciseRect Fractal::GetPreciseViewForPixelRect(const sf::Rect<int>& pixelCoordinates) const
{
    EnsurePreciseViewInitialized();
    const HighPrecisionReal xFactor = (_preciseView.right - _preciseView.left) / HighPrecisionReal(_screenWidth);
    const HighPrecisionReal yFactor = (_preciseView.top - _preciseView.bottom) / HighPrecisionReal(_screenHeight);

    PreciseRect view;
    view.right = _preciseView.left + HighPrecisionReal(pixelCoordinates.left + pixelCoordinates.width) * xFactor;
    view.left = _preciseView.left + HighPrecisionReal(pixelCoordinates.left) * xFactor;
    view.bottom = _preciseView.top - HighPrecisionReal(pixelCoordinates.top + pixelCoordinates.height) * yFactor;
    view.top = view.bottom + (view.right - view.left) * HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    return view;
}

Rect Fractal::GetExpandedView(const double scale) const
{
    return GetPreciseExpandedView(scale).ToRect();
}

PreciseRect Fractal::GetPreciseExpandedView(const double scale) const
{
    EnsurePreciseViewInitialized();
    PreciseRect view = _preciseView;
    const HighPrecisionReal scaleX = RealAbs(view.right - view.left) * HighPrecisionReal(scale);
    const HighPrecisionReal scaleY = RealAbs(view.top - view.bottom) * HighPrecisionReal(scale);

    view.left -= scaleX;
    view.right += scaleX;
    view.bottom -= scaleY;
    view.top = view.bottom + (view.right - view.left) * HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    return view;
}

Rect Fractal::GetCenteredView(const double x, const double y, const double radius) const
{
    return GetPreciseCenteredView(HighPrecisionReal(x), HighPrecisionReal(y), HighPrecisionReal(radius)).ToRect();
}

PreciseRect Fractal::GetPreciseCenteredView(const HighPrecisionReal& x, const HighPrecisionReal& y, const HighPrecisionReal& radius) const
{
    const HighPrecisionReal verticalRadius = radius * HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    return {x - radius, y - verticalRadius, x + radius, y + verticalRadius};
}

void Fractal::PanViewByPixels(const int pixelDeltaX, const int pixelDeltaY)
{
    EnsurePreciseViewInitialized();
    const HighPrecisionReal fx = (_preciseView.right - _preciseView.left) / HighPrecisionReal(_screenWidth);
    const HighPrecisionReal fy = (_preciseView.top - _preciseView.bottom) / HighPrecisionReal(_screenHeight);

    _preciseView.left -= HighPrecisionReal(pixelDeltaX) * fx;
    _preciseView.right -= HighPrecisionReal(pixelDeltaX) * fx;
    _preciseView.bottom += HighPrecisionReal(pixelDeltaY) * fy;
    _preciseView.top += HighPrecisionReal(pixelDeltaY) * fy;
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();
}

bool Fractal::IsRendered() const
{
    return _rendered;
}

bool Fractal::IsRenderStarted() const
{
    return _rendering;
}

void Fractal::MarkRenderStarted()
{
    _rendering = true;
}

void Fractal::MarkRenderComplete()
{
    _rendered = true;
    _rendering = false;
}

void Fractal::MarkRenderDirty()
{
    _rendered = false;
}

void Fractal::MarkRenderInterrupted()
{
    _rendered = false;
    _rendering = false;
    _paused = false;
}

void Fractal::MarkRenderAborted()
{
    _rendered = true;
    _rendering = false;
    _paused = false;
    _redrawAll = true;
}

void Fractal::ResumeFromPausedPan()
{
    _rendering = false;
    _rendered = false;
    _paused = false;
}

bool Fractal::IsPausedForPresentation() const
{
    return _paused;
}

bool Fractal::ShouldResumeFromPausedPan() const
{
    return _paused && !_pausing;
}

bool Fractal::ConsumePausePresentationRefresh()
{
    if (!_pausing)
        return false;

    _pausing = false;
    _refreshImage = false;
    return true;
}

bool Fractal::ConsumeImageRefreshRequest()
{
    if (!_refreshImage)
        return false;

    _refreshImage = false;
    return true;
}

sf::Color Fractal::GetInvalidPixelColor() const
{
    return GetColorFromPalette(_changeGradient);
}

bool Fractal::IsExteriorColorEnabled() const
{
    return _colorMode;
}

bool Fractal::IsRelativeColorEnabled() const
{
    return _relativeColor;
}

bool Fractal::IsSetColorEnabled() const
{
    return _colorSet;
}

bool Fractal::IsAntiAliasingEnabled() const
{
    return GetAntiAliasingScale() > 1;
}

bool Fractal::IsGradientAnimating() const
{
    return _colorRotation;
}

bool Fractal::ConsumeGradientChangeRequest()
{
    const bool changed = _varGradChange;
    _varGradChange = false;
    return changed;
}

void Fractal::AdvanceGradientOffset(const double elapsedSeconds)
{
    if (_paletteSize == 0 || elapsedSeconds <= 0.0 || _colorRotationSpeed <= 0.0)
        return;

    constexpr double maxAnimationStepSeconds = 0.25;
    const double colorMovement = _colorRotationSpeed * std::min(elapsedSeconds, maxAnimationStepSeconds) + _colorRotationRemainder;
    const auto offset = static_cast<unsigned int>(std::floor(colorMovement));
    _colorRotationRemainder = colorMovement - offset;
    if (offset == 0)
        return;

    _changeGradient = (_changeGradient + offset) % _paletteSize;
}

bool Fractal::ShouldDrawOrbit() const
{
    return _orbitMode;
}

bool Fractal::IsOrbitDrawn() const
{
    return _orbitDrawn;
}

void Fractal::MarkOrbitDirty()
{
    _orbitDrawn = false;
}

bool Fractal::HasGeometryFigures() const
{
    return _geomFigure;
}

bool Fractal::IsSnapshotActive() const
{
    return _onSnapshot;
}

const std::vector<LineData>& Fractal::GetLines() const
{
    return _lines;
}

const std::vector<CircleData>& Fractal::GetCircles() const
{
    return _circles;
}

wxString Fractal::DescribeOrbit(const bool escaped) const
{
    const std::vector<LineData>& orbitLines = GetOrbitLines();
    if (orbitLines.empty())
        return "Orbit: no transitions were recorded.";

    const double startRe = orbitLines.front().x1;
    const double startIm = orbitLines.front().y1;
    const double finalRe = orbitLines.back().x2;
    const double finalIm = orbitLines.back().y2;

    double totalDistance = 0.0;
    double largestStep = 0.0;
    double closestToOrigin = hypot(startRe, startIm);
    double farthestFromOrigin = closestToOrigin;

    for (const LineData& line : orbitLines)
    {
        const double stepDistance = hypot(line.x2 - line.x1, line.y2 - line.y1);
        const double distanceToOrigin = hypot(line.x2, line.y2);
        totalDistance += stepDistance;
        largestStep = std::max(largestStep, stepDistance);
        closestToOrigin = std::min(closestToOrigin, distanceToOrigin);
        farthestFromOrigin = std::max(farthestFromOrigin, distanceToOrigin);
    }

    const double displacement = hypot(finalRe - startRe, finalIm - startIm);
    const double averageStep = totalDistance / static_cast<double>(orbitLines.size());
    const double pathEfficiency = totalDistance > 0.0 ? 100.0 * displacement / totalDistance : 0.0;
    const double finalModulus = hypot(finalRe, finalIm);
    constexpr double radiansToDegrees = 180.0 / 3.14159265358979323846;
    const double finalAngle = atan2(finalIm, finalRe) * radiansToDegrees;

    wxString output;
    output << "Orbit transitions: " << orbitLines.size() << "\n"
           << "Orbit path length: " << FormatNumber(totalDistance) << "\n"
           << "Straight-line displacement: " << FormatNumber(displacement)
           << " (" << FormatNumber(pathEfficiency) << "% of path length)\n"
           << "Average / largest step: " << FormatNumber(averageStep) << " / "
           << FormatNumber(largestStep) << "\n"
           << "Closest / farthest from origin: " << FormatNumber(closestToOrigin) << " / "
           << FormatNumber(farthestFromOrigin) << "\n"
           << (escaped ? "Escape value: " : "Last recorded value: ")
           << FormatComplex(finalRe, finalIm) << "\n"
           << "Final modulus / angle: " << FormatNumber(finalModulus) << " / "
           << FormatNumber(finalAngle) << " degrees";
    return output;
}

wxString Fractal::FormatNumber(const double value)
{
    return wxString::Format("%.10g", value);
}

wxString Fractal::FormatComplex(const double real, const double imaginary)
{
    return FormatNumber(real) + (imaginary < 0.0 ? " - " : " + ")
           + FormatNumber(abs(imaginary)) + "i";
}

void Fractal::CreateInspectionFractal(FractalFactory& factory, unsigned int width, unsigned int height) const
{
    factory.CreateFractal(_type, width, height);
}

void Fractal::RenderBlocking()
{
    const bool previousSnapshot = _onSnapshot;
    const bool previousWaitRoutine = _waitRoutine;
    _onSnapshot = true;
    _waitRoutine = true;

    this->PrepareRender();
    this->Render();
    this->PreDrawMaps();

    _rendered = true;
    _rendering = false;
    _onSnapshot = previousSnapshot;
    _waitRoutine = previousWaitRoutine;
}

wxString Fractal::InspectPoint(const double x, const double y, const optional<unsigned int> iterations) const
{
    constexpr unsigned int probeSize = 3;
    FractalFactory probeHandler;
    CreateInspectionFractal(probeHandler, probeSize, probeSize);

    Fractal* probe = probeHandler.GetFractal();
    if (probe == nullptr)
        return "This fractal cannot be inspected.";

    Options options = GetOptions();
    const vector<RenderingAlgorithmType> algorithms = probe->GetAvailableAlg();
    if (find(algorithms.begin(), algorithms.end(), RenderingAlgorithmType::EscapeTime) != algorithms.end())
        options.alg = RenderingAlgorithmType::EscapeTime;
    else if (!algorithms.empty())
        options.alg = algorithms.front();

    options.orbitTrapMode = false;
    options.smoothRender = false;
    options.antiAliasingScale = 1;
    if (iterations.has_value())
        options.maxIterations = *iterations;

    probe->SetOptions(options);
    probe->SetFormula(_userFormula);

    const double scale = max({1.0, abs(x), abs(y)});
    const double epsilon = scale * 1e-10;
    probe->SetView({x - epsilon, y - epsilon, x + epsilon, y + epsilon});
    probe->RenderBlocking();

    const PointSample sample = probe->GetPointSample(1, 1);
    wxString output;
    output << "Fractal: " << probe->GetName() << "\n"
           << "Coordinates: (" << FormatNumber(x) << ", " << FormatNumber(y) << ")\n"
           << "Algorithm: " << probe->GetRenderingAlgorithmName() << "\n"
           << "Maximum iterations: " << options.maxIterations << "\n";

    if (sample.inSet)
        output << "Result: inside after " << options.maxIterations << " iterations";
    else if (sample.hasValue && options.alg == RenderingAlgorithmType::EscapeTime)
        output << "Result: escaped at iteration " << sample.value;
    else if (sample.hasValue)
        output << "Renderer value: " << sample.value;
    else
        output << "Result: no value produced";

    if (probe->HasOrbit())
    {
        probe->SetOrbitMode(true);
        probe->SetOrbitPoint(x, y);
        probe->DrawOrbit();
        output << "\n" << probe->DescribeOrbit(!sample.inSet);
    }

    return output;
}
// Thread control
bool Fractal::IsPaused() const
{
    return _paused;
}
// Virtual methods.
void Fractal::PreRender() {}
void Fractal::PreDrawMaps() {}
void Fractal::PostRender() {}
void Fractal::PreRestartRender() {}
void Fractal::SetFormula(FormulaOptions formula)
{
    _userFormula = std::move(formula);
}
void Fractal::CopyOptionFromPanel() {}

// Communication methods.
double Fractal::GetX(const int pixelX) const
{
    return ToDouble(GetPreciseX(pixelX));
}
HighPrecisionReal Fractal::GetPreciseX(const int pixelX) const
{
    EnsurePreciseViewInitialized();
    return _preciseView.left + HighPrecisionReal(pixelX) * _preciseXFactor;
}
double Fractal::GetY(const int pixelY) const
{
    return ToDouble(GetPreciseY(pixelY));
}
HighPrecisionReal Fractal::GetPreciseY(const int pixelY) const
{
    EnsurePreciseViewInitialized();
    return _preciseView.top - HighPrecisionReal(pixelY) * _preciseYFactor;
}
int Fractal::GetPixelX(const double xNum) const
{
    EnsurePreciseViewInitialized();
    return static_cast<int>(ToDouble((HighPrecisionReal(xNum) - _preciseView.left) / _preciseXFactor));
}
int Fractal::GetPixelY(const double yNum) const
{
    EnsurePreciseViewInitialized();
    return static_cast<int>(ToDouble((_preciseView.top - HighPrecisionReal(yNum)) / _preciseYFactor));
}
void Fractal::SetOptions(const Options& opt, const bool keepSize)
{
    const bool usePreciseOptions = OptionsPreciseViewMatchesDoubleView(opt);
    const unsigned int antiAliasingScale = opt.antiAliasingScale;

    if (!keepSize)
    {
        _minX = opt.minX;
        _maxX = opt.maxX;
        _minY = opt.minY;
        _maxY = opt.maxY;

        _preciseView = usePreciseOptions ? opt.preciseView : PreciseRect(Rect(_minX, _minY, _maxX, _maxY));
    }
    else
    {
        EnsurePreciseViewInitialized();
        _maxY = _minY + (_maxX - _minX) * static_cast<double>(_screenHeight) / _screenWidth;
        _preciseView.top = _preciseView.bottom + (_preciseView.right - _preciseView.left) *
            HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    }

    _maxIterations = opt.maxIterations;
    _panelOpt.CopyValuesFrom(opt.panelOpt);
    _changeGradient = opt.colorVariationOffset;
    _colorRotationSpeed = std::max(0.0, opt.colorRotationSpeed);
    _relativeColor = opt.relativeColor;
    _gradPaletteSize = opt.gradPaletteSize;
    _colorCycleLength = opt.colorCycleLength > 0.0 ? opt.colorCycleLength : 72.0;
    _paletteMappingMode = opt.paletteMappingMode;
    _paletteMappingExponent = opt.paletteMappingExponent > 0.0 ? opt.paletteMappingExponent : 1.5;
    if (GetAntiAliasingScale() != antiAliasingScale)
        SetAntiAliasingScale(antiAliasingScale);
    _algorithm = opt.alg;
    _renderingPrecisionMode = opt.renderingPrecisionMode;
    _fSetColor = wxColour(opt.fSetColor.r, opt.fSetColor.g, opt.fSetColor.b, opt.fSetColor.a);

    _gradient = opt.gradient;
    _paletteSize = _gradPaletteSize;
    this->SetGradientSize(_paletteSize);

    if (_hasSmoothRender)
        _smoothRender = opt.smoothRender;

    _kReal = opt.kReal;
    _kImaginary = opt.kImaginary;

    _orbitTrapMode = opt.orbitTrapMode;
    _colorSet = opt.colorSet;
    _colorMode = opt.colorMode;

    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();

    this->CopyOptionFromPanel();
}
Options Fractal::GetOptions() const
{
    EnsurePreciseViewInitialized();
    Options opt;

    opt.minX = _minX;
    opt.maxX = _maxX;
    opt.minY = _minY;
    opt.maxY = _maxY;
    opt.xFactor = _xFactor;
    opt.yFactor = _yFactor;
    opt.maxIterations = _maxIterations;
    opt.colorVariationOffset = _changeGradient;
    opt.smoothRender = _smoothRender;
    opt.alg = _algorithm;
    opt.renderingPrecisionMode = _renderingPrecisionMode;
    opt.gradient = _gradient;
    opt.relativeColor = _relativeColor;
    opt.paletteSize = _paletteSize;
    opt.gradPaletteSize = _gradPaletteSize;
    opt.colorCycleLength = _colorCycleLength;
    opt.colorRotationSpeed = _colorRotationSpeed;
    opt.paletteMappingMode = _paletteMappingMode;
    opt.paletteMappingExponent = _paletteMappingExponent;
    opt.antiAliasingScale = GetAntiAliasingScale();
    opt.panelOpt = _panelOpt;
    opt.type = _type;

    opt.kReal = _kReal;
    opt.kImaginary = _kImaginary;

    opt.orbitTrapMode = _orbitTrapMode;
    opt.colorSet = _colorSet;
    opt.colorMode = _colorMode;

    opt.fSetColor = GetSetColor();

    opt.screenWidth = _screenWidth;
    opt.screenHeight = _screenHeight;
    opt.preciseView = _preciseView;
    opt.preciseXFactor = _preciseXFactor;
    opt.preciseYFactor = _preciseYFactor;
    opt.hasPreciseView = true;
    opt.highPrecisionBits = GetHighPrecisionRenderBits();
    opt.useHighPrecision = ShouldUseHighPrecision();

    return opt;
}
bool Fractal::IsHighPrecisionRenderActive() const
{
    return ShouldUseHighPrecision();
}
unsigned int Fractal::GetHighPrecisionRenderBits() const
{
    if (!SupportsHighPrecisionRender())
        return 0;

    const unsigned int precisionBits = EstimateRequiredPrecisionBits();
    if (_renderingPrecisionMode == RenderingPrecisionMode::Precise)
        return std::max(precisionBits, 64U);

    if (_renderingPrecisionMode == RenderingPrecisionMode::Adaptative && precisionBits > 53)
        return precisionBits;

    return 0;
}
void Fractal::SetPrecisionStatusChangedCallback(std::function<void(bool, unsigned int)> callback)
{
    _precisionStatusChanged = std::move(callback);
}
void Fractal::SetRendered(const bool mode)
{
    _rendered = mode;
}
FractalType Fractal::GetType() const
{
    return _type;
}
bool Fractal::HasFractalInformation() const
{
    return FractalDocumentation::HasDocumentation(_type);
}
wxString Fractal::GetFractalInformationFile() const
{
    return FractalDocumentation::GetDocumentFile(_type);
}
void Fractal::SetFractalPropChanged()
{
    _changeFractalProp = true;
}
bool Fractal::GetChangeFractalProp()
{
    bool temp = _changeFractalProp;
    _changeFractalProp = false;
    return temp;
}
// Save image.
wxBitmap Fractal::GetRenderedWxBitmap()
{
    const sf::Image renderedImage = this->GetRenderedImage();
    const sf::Vector2u imageSize = renderedImage.getSize();
    const sf::Uint8* rgbaPixels = renderedImage.getPixelsPtr();
    auto* rgbPixels = new unsigned char[imageSize.x * imageSize.y * 3];

    // wxImage expects packed RGB data, while SFML exposes RGBA pixels.
    for (unsigned int i = 0, j = 0; i < imageSize.x * imageSize.y * 4; i += 4, j += 3)
    {
        rgbPixels[j] = rgbaPixels[i];
        rgbPixels[j + 1] = rgbaPixels[i + 1];
        rgbPixels[j + 2] = rgbaPixels[i + 2];
    }

    const wxImage wxImage(static_cast<int>(imageSize.x), static_cast<int>(imageSize.y), rgbPixels, true);
    wxBitmap output(wxImage);
    return output;
}
void Fractal::PrepareSnapshot(const bool mode)
{
    _onSnapshot = mode;
}
void Fractal::SetColorPalette(const ColorPaletteTypes gradStyle)
{
    _gradStyle = gradStyle;
}

ColorPaletteTypes Fractal::GetColorPalette() const
{
    return _gradStyle;
}
sf::Color Fractal::GetSetColor() const
{
    return {_fSetColor.Red(), _fSetColor.Green(), _fSetColor.Blue(), _fSetColor.Alpha()};
}
// Gradient color.
wxGradient* Fractal::GetGradient()
{
    return &_gradient;
}

void Fractal::SetExteriorColorMode(const bool mode)
{
    // Changes external color mode.
    if (_colorMode != mode)
    {
        _colorMode = mode;
        this->RedrawMaps();
    }
}
void Fractal::SetFractalSetColorMode(const bool mode)
{
    // Changes internal color mode.
    if (_colorSet != mode)
    {
        _colorSet = mode;
        this->RedrawMaps();
    }
}
void Fractal::SetFractalSetColor(const sf::Color color)
{
    // Changes the color of the set.
    _fSetColor = wxColour(color.r, color.g, color.b, color.a);
    this->RedrawMaps();
}
bool Fractal::GetExteriorColorMode() const
{
    return _colorMode;
}
bool Fractal::GetInteriorColorMode() const
{
    return _colorSet;
}
void Fractal::ToggleColorRotation()
{
    _colorRotation = !_colorRotation;
}
void Fractal::SetPaletteSize(const unsigned int size)
{
    this->SetGradientSize(size);
}
unsigned int Fractal::GetPaletteSize() const
{
    return _paletteSize;
}
void Fractal::SetGradient(const wxGradient& grad)
{
    // Copy gradient.
    _gradient = grad;
    _gradPaletteSize = _paletteSize = _gradient.GetMax() - _gradient.GetMin();
    _palette.resize(_paletteSize);
    this->RebuildPalette();
}
void Fractal::SetGradientSize(const unsigned int size)
{
    _gradient.SetMax(size);
    _gradPaletteSize = _paletteSize = size;
    _palette.resize(_paletteSize);
    this->RebuildPalette();
}
void Fractal::SetColorCycleLength(const double cycleLength)
{
    if (cycleLength <= 0.0)
        return;

    _colorCycleLength = cycleLength;
    RedrawMaps();
}
double Fractal::GetColorCycleLength() const
{
    return _colorCycleLength;
}
void Fractal::SetColorRotationSpeed(const double speed)
{
    if (speed < 0.0)
        return;

    _colorRotationSpeed = speed;
    _colorRotationRemainder = 0.0;
}
double Fractal::GetColorRotationSpeed() const
{
    return _colorRotationSpeed;
}
void Fractal::SetPaletteMappingMode(const PaletteMappingMode mode)
{
    _paletteMappingMode = mode;
    RedrawMaps();
}
PaletteMappingMode Fractal::GetPaletteMappingMode() const
{
    return _paletteMappingMode;
}
void Fractal::SetPaletteMappingExponent(const double exponent)
{
    if (exponent <= 0.0)
        return;

    _paletteMappingExponent = exponent;
    RedrawMaps();
}
double Fractal::GetPaletteMappingExponent() const
{
    return _paletteMappingExponent;
}
// RelativeColor.
void Fractal::SetRelativeColor(const bool mode)
{
    _relativeColor = mode;
    this->RebuildPalette();
}
bool Fractal::GetRelativeColorMode() const
{
    return _relativeColor;
}
void Fractal::SetVarGradient(const unsigned int n)
{
    _varGradChange = true;
    if (_paletteSize == 0)
        return;
    _changeGradient = n % _paletteSize;
}

// Algorithm.
RenderingAlgorithmType Fractal::GetCurrentAlg() const
{
    return _algorithm;
}
vector<RenderingAlgorithmType> Fractal::GetAvailableAlg()
{
    return _availableAlg;
}
vector<RenderingPrecisionMode> Fractal::GetAvailableRenderingPrecisionModes() const
{
    vector<RenderingPrecisionMode> modes;
    if (!SupportsHighPrecisionRender())
        return modes;

    modes.push_back(RenderingPrecisionMode::Adaptative);
    modes.push_back(RenderingPrecisionMode::Precise);
    if (SupportsFastPrecisionRender())
        modes.push_back(RenderingPrecisionMode::Fast);

    return modes;
}
void Fractal::SetAlgorithm(const RenderingAlgorithmType algorithm)
{
    _algorithm = algorithm;
    _relativeColor = RenderingAlgorithm::UsesRelativeColorByDefault(_algorithm);
    this->StopRender();
    _rendered = false;
    _rendering = false;
}
void Fractal::SetRenderingPrecisionMode(const RenderingPrecisionMode mode)
{
    if (_renderingPrecisionMode == mode)
        return;

    _renderingPrecisionMode = mode;
    this->StopRender();
    _rendered = false;
    _rendering = false;
}
RenderingPrecisionMode Fractal::GetRenderingPrecisionMode() const
{
    return _renderingPrecisionMode;
}

bool Fractal::IsJuliaVariety() const
{
    return _juliaVariety;
}
void Fractal::SetJuliaMode(const bool mode)
{
    _juliaMode = _waitRoutine = mode;
}

// Julia mode operations.
void Fractal::SetK(const double real, const double imaginary)
{
    this->StopRender();

    if (real != _kReal || imaginary != _kImaginary)
        _rendered = false;

    _kReal = real;
    _kImaginary = imaginary;
}
double Fractal::GetKReal() const
{
    return _kReal;
}
double Fractal::GetKImaginary() const
{
    return _kImaginary;
}
// Orbit mode operations.
void Fractal::SetOrbitMode(const bool mode)
{
    if (_hasOrbit)
    {
        _orbitMode = mode;
        _orbitDrawn = false;
        _orbitX = 0;
        _orbitY = 0;
        ClearOrbitLines();
    }
}
void Fractal::SetOrbitPoint(const double x, const double y)
{
    if (!_orbitDrawn)
    {
        _orbitX = x;
        _orbitY = y;
    }
}

bool Fractal::HasOrbit() const
{
    return _hasOrbit;
}
void Fractal::SetOrbitChange()
{
    _orbitDrawn = false;
}
// Orbit trap operations.
void Fractal::SetOrbitTrapMode(const bool mode)
{
    if (_hasOrbitTrap)
    {
        _orbitTrapMode = mode;
    }
}
bool Fractal::HasOrbitTrapMode() const
{
    return _hasOrbitTrap;
}
bool Fractal::OrbitTrapActivated() const
{
    return _orbitTrapMode;
}
// SmoothRender
void Fractal::SetSmoothRender(const bool mode)
{
    if (_hasSmoothRender)
        _smoothRender = mode;
}

bool Fractal::HasSmoothRenderMode() const
{
    return _hasSmoothRender;
}
bool Fractal::SmoothRenderActivated() const
{
    return _smoothRender;
}
void Fractal::SetIterations(const unsigned int iterations)
{
    _redrawAll = true;
    _maxIterations = iterations;
    _rendered = false;
}

unsigned int Fractal::GetIterations() const
{
    return _maxIterations;
}
unsigned int Fractal::GetDefaultIterations() const
{
    return _defaultIter;
}
unsigned int Fractal::GetIterationStep() const
{
    return _iterationStep;
}
// Option panel.
bool Fractal::HasOptionsPanel() const
{
    return _panelOpt.GetElementsSize() > 0;
}
PanelOptions* Fractal::GetOptionsPanel()
{
    return &_panelOpt;
}

// Geometry operations.
void Fractal::DrawLine(const double x1, const double y1, const double x2, const double y2, const sf::Color color,
                       const bool orbitLine)
{
    LineData data;
    data.x1 = x1;
    data.x2 = x2;
    data.y1 = y1;
    data.y2 = y2;
    data.color = color;

    if (orbitLine)
        return;

    _lines.push_back(data);

    _geomFigure = true;
}

void Fractal::DrawCircle(const double xCenter, const double yCenter, const double radius, const sf::Color color)
{
    this->DrawCircle(xCenter, yCenter, radius, color, false);
}

void Fractal::DrawCircle(const double xCenter, const double yCenter, const double radius, const sf::Color color,
                         const bool filled)
{
    CircleData data;
    data.xCenter = xCenter;
    data.yCenter = yCenter;
    data.radius = radius;
    data.color = color;
    data.filled = filled;
    _circles.push_back(data);
    _geomFigure = true;
}

void Fractal::ClearGeometryFigures()
{
    _circles.clear();
    _lines.clear();
    _geomFigure = !GetOrbitLines().empty();
}
