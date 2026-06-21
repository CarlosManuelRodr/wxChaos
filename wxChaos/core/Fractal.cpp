#include <complex>
#include <algorithm>
#include <limits>
#include <utility>
#include "Fractal.h"
#include "FractalHandler.h"
#include "fractals/ScriptFractal.h"
#include "BmpImageWriter.h"
#include "SystemUtilities.h"
using namespace std;

constexpr ColorPaletteTypes defaultGradientStyle = Retro;
const wxString defaultGradientString = wxT("rgb(4,108,164);rgb(136,171,14);rgb(255,255,255);rgb(171,27,27);rgb(61,43,94);rgb(4,108,164);");

inline double CalcSquaredDist(const double x1, const double y1, const double x2, const double y2)
{
    return ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

inline double CalcDist(const double x1, const double y1, const double x2, const double y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

Fractal::Fractal(const unsigned int width, const unsigned int height) : _pendingRenderOffset(Vector2Int::Zero())
{
    // System.
    _threadNumber = max(Get_Cores() - 1, 1);

    // Copy window properties.
    _screenHeight = height;
    _screenWidth = width;
    _backScreenWidth = _screenWidth;

    _fSetColor = wxColour(0, 0, 0);

    // Allocates memory for the maps.
    _setMap = new bool* [_screenWidth];
    _colorMap = new unsigned int* [_screenWidth];
    _auxMap = new unsigned int* [_screenWidth];
    for (int i = 0; i < _screenWidth; i++)
    {
        _setMap[i] = new bool[_screenHeight];
        _colorMap[i] = new unsigned int[_screenHeight];
        _auxMap[i] = new unsigned int[_screenHeight];
    }

    // Initializes maps.
    for (int i = 0; i < _screenWidth; i++)
    {
        for (int j = 0; j < _screenHeight; j++)
        {
            _setMap[i][j] = false;
            _colorMap[i][j] = InvalidColor;
            _auxMap[i][j] = 0;
        }
    }

    _pendingRenderOffset = {0, 0};

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
    _varGradient = false;
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
    _waitRoutine = false;
    _redrawAll = false;
    _redrawAlways = false;
    _rendering = false;
    _paused = false;
    _pausing = false;
    _justLaunchThreads = false;
    _maxIter = 100;
    _varGradChange = false;
    _refreshImage = false;
    _maxColorMapVal = 0;
    _renderJobCompatible = false;
    _orbitX = _orbitY = 0.0;
    _renderJobCompatible = true;
    _changeFractalProp = false;
    _reportedHighPrecisionActive = false;
    _reportedHighPrecisionBits = 0;
    _geomFigure = false;

    // Creates default color palette.
    _relativeColor = false;
    _gradPaletteSize = _paletteSize = 300;
    _algorithm = RenderingAlgorithmType::Other;
    _gradStyle = defaultGradientStyle;
    _gradient.FromString(defaultGradientString);
    _gradient.SetMin(0);
    _gradient.SetMax(_gradPaletteSize);

    _palette.resize(_paletteSize);
    _varGradientStep = _paletteSize / 60;
    this->RebuildPalette();
}

Fractal::~Fractal()
{
    // Cleanup.
    for (int i = 0; i < _backScreenWidth; i++)
    {
        delete[] _setMap[i];
        delete[] _colorMap[i];
        delete[] _auxMap[i];
    }

    delete[] _setMap;
    delete[] _colorMap;
    delete[] _auxMap;
}

wxString Fractal::GetRenderingAlgorithmName() const
{
    return Renderer::GetAlgorithmName(_algorithm);
}

sf::Color Fractal::GetColorFromPalette(unsigned int index) const
{
    if (index <= 0)
        index = 0;

    index = index % _paletteSize;
    const wxColour& color = _palette[index];
    return {color.Red(), color.Green(), color.Blue(), color.Alpha()};
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
void Fractal::RedrawMaps()
{
    this->UpdateMaxColorMapValue();
    _refreshImage = true;
}

void Fractal::UpdateMaxColorMapValue()
{
    _maxColorMapVal = 0;

    if (_relativeColor)
    {
        // Search for color maximum.
        for (int i = 0; i < _screenWidth; i++)
        {
            for (int j = 0; j < _screenHeight; j++)
            {
                if (_colorMap[i][j] != InvalidColor && _colorMap[i][j] > _maxColorMapVal)
                    _maxColorMapVal = _colorMap[i][j];
            }
        }
    }
    if (_maxColorMapVal == 0)
        _maxColorMapVal = 1;
}
void Fractal::ConfigureRenderer(Renderer& renderer) const
{
    renderer.SetOptions(this->GetOptions());
    renderer.SetRenderOut(_setMap, _colorMap, _auxMap);
    renderer.SetK(_kReal, _kImaginary);
}
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
    return SupportsHighPrecisionRender() && EstimateRequiredPrecisionBits() > 53;
}
bool Fractal::SupportsHighPrecisionRender() const
{
    if (_algorithm == RenderingAlgorithmType::Buddhabrot)
        return false;

    switch (_type)
    {
        case FractalType::Mandelbrot:
        case FractalType::MandelbrotZN:
        case FractalType::Julia:
        case FractalType::JuliaZN:
        case FractalType::NewtonRaphsonMethod:
        case FractalType::Sinoidal:
        case FractalType::Magnetic:
        case FractalType::Medusa:
        case FractalType::Manowar:
        case FractalType::ManowarJulia:
        case FractalType::FixedPoint1:
        case FractalType::FixedPoint2:
        case FractalType::FixedPoint3:
        case FractalType::FixedPoint4:
        case FractalType::Tricorn:
        case FractalType::BurningShip:
        case FractalType::BurningShipJulia:
        case FractalType::Fractory:
        case FractalType::Cell:
            return true;
        default:
            return false;
    }
}
unsigned int Fractal::EstimateRequiredPrecisionBits() const
{
    EnsurePreciseViewInitialized();

    if (_screenWidth <= 1 || _screenHeight <= 1)
        return 0;

    using std::ceil;
    using std::log;
    using std::max;

    const HighPrecisionReal minStep = std::min(HighPrecisionReal(RealAbs(_preciseXFactor)),
                                               HighPrecisionReal(RealAbs(_preciseYFactor)));
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
std::vector<RenderRegion> Fractal::BuildRenderRegions() const
{
    std::vector<RenderRegion> regions;
    const int screenWidth = static_cast<int>(_screenWidth);
    const int screenHeight = static_cast<int>(_screenHeight);

    if (_pendingRenderOffset.x == 0 && _pendingRenderOffset.y == 0)
    {
        regions.emplace_back(0, 0, screenWidth, screenHeight);
        return regions;
    }

    if ((abs(_pendingRenderOffset.x) >= screenWidth) || (abs(_pendingRenderOffset.y) >= screenHeight))
    {
        regions.emplace_back(0, 0, screenWidth, screenHeight);
        return regions;
    }

    int yStart = 0;
    int yEnd = screenHeight;

    if (_pendingRenderOffset.y > 0)
    {
        regions.emplace_back(0, 0, screenWidth, _pendingRenderOffset.y);
        yStart = _pendingRenderOffset.y;
    }
    else if (_pendingRenderOffset.y < 0)
    {
        yEnd = screenHeight + _pendingRenderOffset.y;
        regions.emplace_back(0, yEnd, screenWidth, screenHeight);
    }

    if (_pendingRenderOffset.x > 0)
    {
        regions.emplace_back(0, yStart, _pendingRenderOffset.x, yEnd);
    }
    else if (_pendingRenderOffset.x < 0)
    {
        regions.emplace_back(screenWidth + _pendingRenderOffset.x, yStart, screenWidth, yEnd);
    }

    return regions;
}

std::vector<RenderJob> Fractal::BuildRenderJobs(const std::vector<RenderRegion>& regions, const int tileHeight) const
{
    std::vector<RenderJob> jobs;

    const unsigned int threadNumber = std::max(1U, _threadNumber);
    const int screenWidth = static_cast<int>(_screenWidth);
    const int screenHeight = static_cast<int>(_screenHeight);
    int totalArea = 0;

    if (tileHeight <= 0 && regions.size() > threadNumber)
    {
        jobs.emplace_back(RenderRegion(0, 0, screenWidth, screenHeight));

        while (jobs.size() < threadNumber)
            jobs.emplace_back();

        return jobs;
    }

    for (const RenderRegion& region : regions)
        totalArea += region.GetArea();

    if (totalArea == 0)
    {
        while (jobs.size() < threadNumber)
            jobs.emplace_back();

        return jobs;
    }

    if (tileHeight > 0)
    {
        for (const RenderRegion& region : regions)
        {
            for (int top = region.GetTop(); top < region.GetBottom(); top += tileHeight)
            {
                const int bottom = std::min(top + tileHeight, region.GetBottom());
                jobs.emplace_back(RenderRegion(region.GetLeft(), top, region.GetRight(), bottom));
            }
        }

        return jobs;
    }

    unsigned int remainingJobs = threadNumber;
    int remainingArea = totalArea;

    for (unsigned int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
    {
        const RenderRegion& region = regions[regionIndex];
        const auto remainingRegions = static_cast<unsigned int>(regions.size() - regionIndex);
        unsigned int regionJobs = 1;

        if (remainingRegions == 1)
        {
            regionJobs = remainingJobs;
        }
        else if (region.GetArea() > 0)
        {
            regionJobs = static_cast<unsigned int>(std::max(1.0,
                                                            std::round(static_cast<double>(remainingJobs) * region.GetArea() / remainingArea)));
            regionJobs = std::min(regionJobs, remainingJobs - remainingRegions + 1);
        }

        remainingJobs -= regionJobs;
        remainingArea -= region.GetArea();

        int currentTop = region.GetTop();
        const int height = region.GetHeight();

        for (unsigned int jobIndex = 0; jobIndex < regionJobs; jobIndex++)
        {
            const unsigned int remainingRegionJobs = regionJobs - jobIndex;
            const int rows = remainingRegionJobs > 0
                                 ? (region.GetBottom() - currentTop) / static_cast<int>(remainingRegionJobs)
                                 : 0;
            const int bottom = jobIndex + 1 == regionJobs ? region.GetBottom() : currentTop + rows;

            if (height <= 0 || rows <= 0)
            {
                jobs.emplace_back();
            }
            else
            {
                jobs.emplace_back(RenderRegion(region.GetLeft(), currentTop, region.GetRight(), bottom));
                currentTop = bottom;
            }
        }
    }

    while (jobs.size() < threadNumber)
        jobs.emplace_back();

    return jobs;
}

void Fractal::Resize(const unsigned int width, const unsigned int height)
{
    // Stop threads if they are still rendering.
    this->StopRender();
    _paused = false;

    // Frees memory.
    for (int i = 0; i < _backScreenWidth; i++)
    {
        delete[] _setMap[i];
        delete[] _colorMap[i];
        delete[] _auxMap[i];
        _setMap[i] = nullptr;
        _colorMap[i] = nullptr;
        _auxMap[i] = nullptr;
    }
    delete[] _setMap;
    delete[] _colorMap;
    delete[] _auxMap;
    _setMap = nullptr;
    _colorMap = nullptr;
    _auxMap = nullptr;

    // Copy window properties.
    _screenHeight = height;
    _backScreenWidth = _screenWidth = width;

    // Allocate memory.
    _setMap = new bool* [_screenWidth];
    _colorMap = new unsigned int* [_screenWidth];
    _auxMap = new unsigned int* [_screenWidth];
    for (int i = 0; i < _screenWidth; i++)
    {
        _setMap[i] = new bool[_screenHeight];
        _colorMap[i] = new unsigned int[_screenHeight];
        _auxMap[i] = new unsigned int[_screenHeight];
    }

    // Initialize maps.
    for (int i = 0; i < _screenWidth; i++)
    {
        for (int j = 0; j < _screenHeight; j++)
        {
            _setMap[i][j] = false;
            _colorMap[i][j] = InvalidColor;
            _auxMap[i][j] = 0;
        }
    }

    EnsurePreciseViewInitialized();
    _preciseView.top = _preciseView.bottom + (_preciseView.right - _preciseView.left) *
        HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();

}
void Fractal::PrepareRender(const Vector2Int reusedMapOffset)
{
    this->PreRender();
    _pendingRenderOffset = reusedMapOffset;
    NotifyPrecisionStatusIfChanged();

    // Checks if the movement is valid.
    if ((abs(_pendingRenderOffset.x) >= _screenWidth) || (abs(_pendingRenderOffset.y) >= _screenHeight))
        _redrawAll = true;

    // Clear maps.
    if ((!_pendingRenderOffset.x && !_pendingRenderOffset.y) || _redrawAll || _redrawAlways)
    {
        for (int i = 0; i < _screenWidth; i++)
        {
            for (int j = 0; j < _screenHeight; j++)
            {
                _setMap[i][j] = false;
                _colorMap[i][j] = InvalidColor;
                _auxMap[i][j] = 0;
            }
        }
        _pendingRenderOffset = {0, 0};
        _redrawAll = false;
    }
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

    _pendingRenderOffset = {0, 0};
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
    _pendingRenderOffset = {0, 0};
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

void Fractal::ReuseRenderedMaps(const Vector2Int reusedMapOffset)
{
    MoveMatrix<bool>(_setMap, _screenHeight, _screenWidth, reusedMapOffset.y, reusedMapOffset.x);
    MoveMatrix<unsigned int>(_colorMap, _screenHeight, _screenWidth, reusedMapOffset.y, reusedMapOffset.x, InvalidColor);
    MoveMatrix<unsigned int>(_auxMap, _screenHeight, _screenWidth, reusedMapOffset.y, reusedMapOffset.x);
}

void Fractal::PrepareDisplayColorLookup()
{
    UpdateMaxColorMapValue();
}

bool Fractal::HasDisplayPixelColor(const unsigned int x, const unsigned int y) const
{
    if (_setMap[x][y] && _colorSet)
        return true;

    return _colorMode && _colorMap[x][y] != InvalidColor;
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

bool Fractal::IsGradientAnimating() const
{
    return _varGradient;
}

bool Fractal::ConsumeGradientChangeRequest()
{
    const bool changed = _varGradChange;
    _varGradChange = false;
    return changed;
}

void Fractal::AdvanceGradientOffset()
{
    if (_changeGradient < _paletteSize)
        _changeGradient += _varGradientStep;
    else
        _changeGradient = 0;
}

void Fractal::RefreshAnimatedColors(sf::Image& image)
{
    UpdateMaxColorMapValue();

    for (unsigned int i = 0; i < _screenWidth; i++)
    {
        for (unsigned int j = 0; j < _screenHeight; j++)
        {
            if ((_setMap[i][j] || !_colorSet) && _colorMap[i][j] == InvalidColor)
                continue;

            if ((_setMap[i][j] == false || !_colorSet) && _colorMap[i][j] != InvalidColor)
                image.setPixel(i, j, GetRenderedPixelColor(i, j));
        }
    }
}

bool Fractal::ShouldDrawOrbit() const
{
    return _orbitMode;
}

bool Fractal::IsOrbitDrawn() const
{
    return _orbitDrawn;
}

void Fractal::ClearOrbitLines()
{
    _orbitLines.clear();
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

const std::vector<LineData>& Fractal::GetOrbitLines() const
{
    return _orbitLines;
}

const std::vector<CircleData>& Fractal::GetCircles() const
{
    return _circles;
}

wxString Fractal::DescribeOrbit(const bool escaped) const
{
    if (_orbitLines.empty())
        return wxT("Orbit: no transitions were recorded.");

    const double startRe = _orbitLines.front().x1;
    const double startIm = _orbitLines.front().y1;
    const double finalRe = _orbitLines.back().x2;
    const double finalIm = _orbitLines.back().y2;

    double totalDistance = 0.0;
    double largestStep = 0.0;
    double closestToOrigin = hypot(startRe, startIm);
    double farthestFromOrigin = closestToOrigin;

    for (const LineData& line : _orbitLines)
    {
        const double stepDistance = hypot(line.x2 - line.x1, line.y2 - line.y1);
        const double distanceToOrigin = hypot(line.x2, line.y2);
        totalDistance += stepDistance;
        largestStep = std::max(largestStep, stepDistance);
        closestToOrigin = std::min(closestToOrigin, distanceToOrigin);
        farthestFromOrigin = std::max(farthestFromOrigin, distanceToOrigin);
    }

    const double displacement = hypot(finalRe - startRe, finalIm - startIm);
    const double averageStep = totalDistance / static_cast<double>(_orbitLines.size());
    const double pathEfficiency = totalDistance > 0.0 ? 100.0 * displacement / totalDistance : 0.0;
    const double finalModulus = hypot(finalRe, finalIm);
    constexpr double radiansToDegrees = 180.0 / 3.14159265358979323846;
    const double finalAngle = atan2(finalIm, finalRe) * radiansToDegrees;

    wxString output;
    output << wxT("Orbit transitions: ") << _orbitLines.size() << wxT("\n")
           << wxT("Orbit path length: ") << FormatNumber(totalDistance) << wxT("\n")
           << wxT("Straight-line displacement: ") << FormatNumber(displacement)
           << wxT(" (") << FormatNumber(pathEfficiency) << wxT("% of path length)\n")
           << wxT("Average / largest step: ") << FormatNumber(averageStep) << wxT(" / ")
           << FormatNumber(largestStep) << wxT("\n")
           << wxT("Closest / farthest from origin: ") << FormatNumber(closestToOrigin) << wxT(" / ")
           << FormatNumber(farthestFromOrigin) << wxT("\n")
           << (escaped ? wxT("Escape value: ") : wxT("Last recorded value: "))
           << FormatComplex(finalRe, finalIm) << wxT("\n")
           << wxT("Final modulus / angle: ") << FormatNumber(finalModulus) << wxT(" / ")
           << FormatNumber(finalAngle) << wxT(" degrees");
    return output;
}

wxString Fractal::FormatNumber(const double value)
{
    return wxString::Format(wxT("%.10g"), value);
}

wxString Fractal::FormatComplex(const double real, const double imaginary)
{
    return FormatNumber(real) + (imaginary < 0.0 ? wxT(" - ") : wxT(" + "))
           + FormatNumber(abs(imaginary)) + wxT("i");
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

Fractal::PointSample Fractal::GetPointSample(const unsigned int x, const unsigned int y) const
{
    if (x >= _screenWidth || y >= _screenHeight)
        return {false, 0, false};

    return {_setMap[x][y], _colorMap[x][y], _colorMap[x][y] != InvalidColor};
}

wxString Fractal::InspectPoint(const double real, const double imaginary,
                               const optional<unsigned int> iterations) const
{
    constexpr unsigned int probeSize = 3;
    FractalHandler probeHandler;

    if (_type == FractalType::ScriptFractal)
    {
        const auto* scriptFractal = dynamic_cast<const ScriptFractal*>(this);
        if (scriptFractal == nullptr)
            return wxT("This fractal cannot be inspected.");
        probeHandler.CreateScriptFractal(probeSize, probeSize, scriptFractal->GetScriptData());
    }
    else
        probeHandler.CreateFractal(_type, probeSize, probeSize);

    Fractal* probe = probeHandler.GetFractalPtr();
    if (probe == nullptr)
        return wxT("This fractal cannot be inspected.");

    Options options = GetOptions();
    const vector<RenderingAlgorithmType> algorithms = probe->GetAvailableAlg();
    if (find(algorithms.begin(), algorithms.end(), RenderingAlgorithmType::EscapeTime) != algorithms.end())
        options.alg = RenderingAlgorithmType::EscapeTime;
    else if (!algorithms.empty())
        options.alg = algorithms.front();

    options.orbitTrapMode = false;
    options.smoothRender = false;
    options.justLaunchThreads = false;
    if (iterations.has_value())
        options.maxIter = *iterations;

    probe->SetOptions(options);
    probe->SetFormula(_userFormula);

    const double scale = max({1.0, abs(real), abs(imaginary)});
    const double epsilon = scale * 1e-10;
    probe->SetView({real - epsilon, imaginary - epsilon, real + epsilon, imaginary + epsilon});
    probe->RenderBlocking();

    const PointSample sample = probe->GetPointSample(1, 1);
    wxString output;
    output << wxT("Fractal: ") << probe->GetName() << wxT("\n")
           << wxT("Point: ") << FormatComplex(real, imaginary) << wxT("\n")
           << wxT("Algorithm: ") << probe->GetRenderingAlgorithmName() << wxT("\n")
           << wxT("Maximum iterations: ") << options.maxIter << wxT("\n");

    if (sample.inSet)
        output << wxT("Result: inside after ") << options.maxIter << wxT(" iterations");
    else if (sample.hasValue && options.alg == RenderingAlgorithmType::EscapeTime)
        output << wxT("Result: escaped at iteration ") << sample.value;
    else if (sample.hasValue)
        output << wxT("Renderer value: ") << sample.value;
    else
        output << wxT("Result: no value produced");

    if (probe->HasOrbit())
    {
        probe->SetOrbitMode(true);
        probe->SetOrbitPoint(real, imaginary);
        probe->DrawOrbit();
        output << wxT("\n") << probe->DescribeOrbit(!sample.inSet);
    }

    return output;
}
// Thread control
ThreadWatchdog<Renderer>* Fractal::GetWatchdog()
{
    return &_watchdog;
}
int Fractal::GetRenderProgress()
{
    if (_renderPool.IsRunning())
        return _renderPool.GetProgress();

    return _watchdog.GetThreadProgress();
}
void Fractal::PauseContinue()
{
    if (_paused)
    {
        this->PreRestartRender();
        _rendered = false;
        _rendering = true;
        if (_renderJobCompatible)
            this->Render();
        else
        {
            _watchdog.LaunchThreads();
            _watchdog.launch();
        }
        _paused = false;
    }
    else
    {
        this->StopRender();
        _rendered = true;
        _paused = true;
        _pausing = true;
    }
}
bool Fractal::StopRender()
{
    if (this->IsRendering())
    {
        if (_renderPool.IsRunning())
            _renderPool.Stop();
        else
        {
            _watchdog.terminate();
            _watchdog.StopThreads();
        }
        _rendering = false;
        return true;
    }
    return false;
}

bool Fractal::IsPaused() const
{
    return _paused;
}
// Virtual methods.
void Fractal::PreRender() {}
void Fractal::PreDrawMaps() {}
void Fractal::PostRender() {}
void Fractal::PreRestartRender() {}
bool Fractal::IsRendering()
{
    if (_waitRoutine)
        return false;
    return _renderPool.IsRunning() || _watchdog.ThreadRunning();
}
void Fractal::SetFormula(FormulaOptions formula)
{
    _userFormula = std::move(formula);
}
void Fractal::CopyOptFromPanel() {}

// Communication methods.
double Fractal::GetX(const int pixelX) const
{
    EnsurePreciseViewInitialized();
    return ToDouble(_preciseView.left + HighPrecisionReal(pixelX) * _preciseXFactor);
}
double Fractal::GetY(const int pixelY) const
{
    EnsurePreciseViewInitialized();
    return ToDouble(_preciseView.top - HighPrecisionReal(pixelY) * _preciseYFactor);
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

    _maxIter = opt.maxIter;
    _panelOpt = opt.panelOpt;
    _changeGradient = opt.changeGradient;
    _relativeColor = opt.relativeColor;
    _gradPaletteSize = opt.gradPaletteSize;
    _algorithm = opt.alg;
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
    _justLaunchThreads = opt.justLaunchThreads;

    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();

    this->CopyOptFromPanel();
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
    opt.maxIter = _maxIter;
    opt.changeGradient = _changeGradient;
    opt.smoothRender = _smoothRender;
    opt.alg = _algorithm;
    opt.gradient = _gradient;
    opt.relativeColor = _relativeColor;
    opt.paletteSize = _paletteSize;
    opt.gradPaletteSize = _gradPaletteSize;
    opt.panelOpt = _panelOpt;
    opt.type = _type;

    opt.kReal = _kReal;
    opt.kImaginary = _kImaginary;

    opt.orbitTrapMode = _orbitTrapMode;
    opt.colorSet = _colorSet;
    opt.colorMode = _colorMode;
    opt.justLaunchThreads = _justLaunchThreads;

    opt.fSetColor = GetSetColor();

    opt.screenWidth = _screenWidth;
    opt.screenHeight = _screenHeight;
    opt.preciseView = _preciseView;
    opt.preciseXFactor = _preciseXFactor;
    opt.preciseYFactor = _preciseYFactor;
    opt.hasPreciseView = true;
    opt.highPrecisionBits = EstimateRequiredPrecisionBits();
    opt.useHighPrecision = SupportsHighPrecisionRender() && opt.highPrecisionBits > 53;

    return opt;
}
bool Fractal::IsHighPrecisionRenderActive() const
{
    return ShouldUseHighPrecision();
}
unsigned int Fractal::GetHighPrecisionRenderBits() const
{
    const unsigned int precisionBits = EstimateRequiredPrecisionBits();
    return SupportsHighPrecisionRender() && precisionBits > 53 ? precisionBits : 0;
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
bool** Fractal::GetSetMap() const
{
    return _setMap;
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
sf::Color Fractal::GetRenderedPixelColor(const unsigned int x, const unsigned int y) const
{
    if (_setMap[x][y] && _colorSet)
        return GetSetColor();

    if (!_colorMode || _colorMap[x][y] == InvalidColor)
        return sf::Color::White;

    if (_relativeColor)
    {
        const double colorMapValue = _colorMap[x][y];
        const auto doubleMaxColorMapValue = static_cast<double>(_maxColorMapVal);
        const double ratio = colorMapValue / doubleMaxColorMapValue;
        return GetColorFromPalette(static_cast<int>(ratio * _paletteSize + _changeGradient));
    }

    return GetColorFromPalette(_colorMap[x][y] + _changeGradient);
}

sf::Image Fractal::GetRenderedImage()
{
    _onSnapshot = true;
    _waitRoutine = true;
    if (!_rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();

    sf::Image image;
    image.create(_screenWidth, _screenHeight, sf::Color(255, 255, 255));

    this->UpdateMaxColorMapValue();

    for (unsigned int i = 0; i < _screenWidth; i++)
    {
        for (unsigned int j = 0; j < _screenHeight; j++)
            image.setPixel(i, j, GetRenderedPixelColor(i, j));
    }

    _onSnapshot = false;
    _waitRoutine = false;
    return image;
}
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
bool Fractal::SaveBmp(const string& filename)
{
    _waitRoutine = true;
    _onSnapshot = true;

    BmpImageWriter writer(filename, _screenWidth, _screenHeight);
    if (!writer.IsOpen())
    {
        _onSnapshot = false;
        _waitRoutine = false;
        return false;
    }

    if (!_rendered)
    {
        this->PrepareRender();
        this->Render();
    }
    this->PreDrawMaps();

    this->UpdateMaxColorMapValue();

    std::vector<BmpPixel> row(_screenWidth);
    bool success = true;
    for (unsigned int y = 0; y < _screenHeight; y++)
    {
        for (unsigned int x = 0; x < _screenWidth; x++)
        {
            const sf::Color color = GetRenderedPixelColor(x, y);
            row[x] = {color.r, color.g, color.b};
        }

        if (!writer.WriteRow(row))
        {
            success = false;
            break;
        }
    }

    success = writer.Close() && success;
    _onSnapshot = false;
    _waitRoutine = false;
    return success;
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
void Fractal::ChangeVarGradient()
{
    _varGradient = !_varGradient;
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
    _varGradientStep = _paletteSize / 60;
    this->RebuildPalette();
}
void Fractal::SetGradientSize(const unsigned int size)
{
    _gradient.SetMax(size);
    _gradPaletteSize = _paletteSize = size;
    _palette.resize(_paletteSize);
    _varGradientStep = _paletteSize / 60;
    this->RebuildPalette();
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
void Fractal::SetAlgorithm(const RenderingAlgorithmType algorithm)
{
    _algorithm = algorithm;
    this->StopRender();
    _rendered = false;
    _rendering = false;
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
        _orbitX = 0;
        _orbitY = 0;
        _orbitLines.clear();
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
    _maxIter = iterations;
    _rendered = false;
}

unsigned int Fractal::GetIterations() const
{
    return _maxIter;
}
// Option panel.
bool Fractal::HasOptPanel() const
{
    return _panelOpt.GetElementsSize() > 0;
}
PanelOptions* Fractal::GetOptPanel()
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
        _orbitLines.push_back(data);
    else
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
    _geomFigure = !_orbitLines.empty();
}
