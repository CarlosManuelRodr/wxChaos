#include <algorithm>
#include <cmath>
#include <limits>
#include <thread>
#include "RasterFractal.h"
#include "BmpImageWriter.h"
#include "../coloring/PaletteMapping.h"
#include "../SystemUtilities.h"
using namespace std;

RasterFractal::RasterFractal(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _threadNumber = max(Platform::ProcessorCount() - 1, 1);
    UpdateRenderDimensions();
    AllocateRenderMaps();
    ClearRenderMaps(InvalidColor);
}

RasterFractal::~RasterFractal()
{
    ReleaseRenderMaps();
}
bool RasterFractal::IsValidColorMapValue(const double value)
{
    return value != InvalidColor && std::isfinite(value);
}

unsigned int RasterFractal::NormalizeAntiAliasingScale(const unsigned int scale)
{
    switch (scale)
    {
        case 2:
        case 4:
            return scale;
        default:
            return 1;
    }
}

void RasterFractal::UpdateRenderDimensions()
{
    const auto scaledWidth = static_cast<unsigned long long>(_screenWidth) * _antiAliasingScale;
    const auto scaledHeight = static_cast<unsigned long long>(_screenHeight) * _antiAliasingScale;
    _renderWidth = static_cast<unsigned int>(
        std::min<unsigned long long>(std::numeric_limits<unsigned int>::max(), std::max(1ULL, scaledWidth)));
    _renderHeight = static_cast<unsigned int>(
        std::min<unsigned long long>(std::numeric_limits<unsigned int>::max(), std::max(1ULL, scaledHeight)));
}

void RasterFractal::AllocateRenderMaps()
{
    _setMap = new bool* [_renderWidth];
    _colorMap = new double* [_renderWidth];
    for (unsigned int i = 0; i < _renderWidth; i++)
    {
        _setMap[i] = new bool[_renderHeight];
        _colorMap[i] = new double[_renderHeight];
    }
    _backRenderWidth = _renderWidth;
}

void RasterFractal::ReleaseRenderMaps()
{
    if (_setMap == nullptr || _colorMap == nullptr)
        return;

    for (unsigned int i = 0; i < _backRenderWidth; i++)
    {
        delete[] _setMap[i];
        delete[] _colorMap[i];
    }

    delete[] _setMap;
    delete[] _colorMap;
    _setMap = nullptr;
    _colorMap = nullptr;
    _backRenderWidth = 0;
}

void RasterFractal::ClearRenderMaps(const double initialColorValue)
{
    for (unsigned int i = 0; i < _renderWidth; i++)
    {
        for (unsigned int j = 0; j < _renderHeight; j++)
        {
            _setMap[i][j] = false;
            _colorMap[i][j] = initialColorValue;
        }
    }
}

Vector2Int RasterFractal::DisplayOffsetToRenderOffset(const Vector2Int displayOffset) const
{
    const auto scale = static_cast<int>(_antiAliasingScale);
    return {displayOffset.x * scale, displayOffset.y * scale};
}

HighPrecisionReal RasterFractal::GetRenderPreciseXFactor() const
{
    EnsurePreciseViewInitialized();
    if (_antiAliasingScale > 1)
        return (_preciseView.right - _preciseView.left) / HighPrecisionReal(_renderWidth);

    const HighPrecisionReal widthDivisor = _renderWidth > 1 ? HighPrecisionReal(_renderWidth - 1) : HighPrecisionReal(1);
    return (_preciseView.right - _preciseView.left) / widthDivisor;
}

HighPrecisionReal RasterFractal::GetRenderPreciseYFactor() const
{
    EnsurePreciseViewInitialized();
    if (_antiAliasingScale > 1)
        return (_preciseView.top - _preciseView.bottom) / HighPrecisionReal(_renderHeight);

    const HighPrecisionReal heightDivisor = _renderHeight > 1 ? HighPrecisionReal(_renderHeight - 1) : HighPrecisionReal(1);
    return (_preciseView.top - _preciseView.bottom) / heightDivisor;
}

Options RasterFractal::GetRenderOptions() const
{
    Options opt = GetOptions();
    opt.screenWidth = _renderWidth;
    opt.screenHeight = _renderHeight;
    opt.xFactor = ToDouble(GetRenderPreciseXFactor());
    opt.yFactor = ToDouble(GetRenderPreciseYFactor());
    opt.preciseXFactor = GetRenderPreciseXFactor();
    opt.preciseYFactor = GetRenderPreciseYFactor();
    return opt;
}

void RasterFractal::RedrawMaps()
{
    this->UpdateMaxColorMapValue();
    _refreshImage = true;
}

void RasterFractal::UpdateMaxColorMapValue()
{
    _maxColorMapVal = 0.0;
    double minColorMapVal = std::numeric_limits<double>::max();
    std::vector<double> relativeValues;

    for (unsigned int i = 0; i < _renderWidth; i++)
    {
        for (unsigned int j = 0; j < _renderHeight; j++)
        {
            const double value = _colorMap[i][j];
            if (!IsValidColorMapValue(value))
                continue;

            minColorMapVal = std::min(minColorMapVal, value);
            _maxColorMapVal = std::max(_maxColorMapVal, value);
            if (_relativeColor)
                relativeValues.push_back(value);
        }
    }

    if (minColorMapVal == std::numeric_limits<double>::max())
    {
        _relativeColorMin = 0.0;
        _relativeColorMax = 1.0;
        _maxColorMapVal = 1.0;
        return;
    }

    _relativeColorMin = minColorMapVal;
    _relativeColorMax = _maxColorMapVal;

    if (_relativeColor && relativeValues.size() > 8)
    {
        const auto percentileValue = [&relativeValues](const double percentile)
        {
            const auto index = static_cast<std::size_t>(std::round(percentile * static_cast<double>(relativeValues.size() - 1)));
            auto nth = relativeValues.begin() + static_cast<std::ptrdiff_t>(std::min(index, relativeValues.size() - 1));
            std::nth_element(relativeValues.begin(), nth, relativeValues.end());
            return *nth;
        };

        _relativeColorMin = percentileValue(0.02);
        _relativeColorMax = percentileValue(0.98);
        if (_relativeColorMax <= _relativeColorMin)
        {
            _relativeColorMin = minColorMapVal;
            _relativeColorMax = _maxColorMapVal;
        }
    }

    if (_relativeColorMax <= _relativeColorMin)
        _relativeColorMax = _relativeColorMin + 1.0;
    if (_maxColorMapVal <= 0.0)
        _maxColorMapVal = 1.0;
}

void RasterFractal::ConfigureRenderer(RasterRenderWorker& renderer) const
{
    renderer.SetOptions(this->GetRenderOptions());
    renderer.SetOutputRenderingMaps(_setMap, _colorMap);
    renderer.SetK(_kReal, _kImaginary);
}

std::vector<RasterRegion> RasterFractal::BuildRenderRegions() const
{
    std::vector<RasterRegion> regions;
    const int screenWidth = static_cast<int>(_renderWidth);
    const int screenHeight = static_cast<int>(_renderHeight);

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

std::vector<RasterJob> RasterFractal::BuildRenderJobs(const std::vector<RasterRegion>& regions, const int tileHeight) const
{
    std::vector<RasterJob> jobs;

    const unsigned int threadNumber = std::max(1U, _threadNumber);
    const int screenWidth = static_cast<int>(_renderWidth);
    const int screenHeight = static_cast<int>(_renderHeight);
    int totalArea = 0;

    if (tileHeight <= 0 && regions.size() > threadNumber)
    {
        jobs.emplace_back(RasterRegion(0, 0, screenWidth, screenHeight));

        while (jobs.size() < threadNumber)
            jobs.emplace_back();

        return jobs;
    }

    for (const RasterRegion& region : regions)
        totalArea += region.GetArea();

    if (totalArea == 0)
    {
        while (jobs.size() < threadNumber)
            jobs.emplace_back();

        return jobs;
    }

    if (tileHeight > 0)
    {
        for (const RasterRegion& region : regions)
        {
            for (int top = region.GetTop(); top < region.GetBottom(); top += tileHeight)
            {
                const int bottom = std::min(top + tileHeight, region.GetBottom());
                jobs.emplace_back(RasterRegion(region.GetLeft(), top, region.GetRight(), bottom));
            }
        }

        return jobs;
    }

    unsigned int remainingJobs = threadNumber;
    int remainingArea = totalArea;

    for (unsigned int regionIndex = 0; regionIndex < regions.size(); regionIndex++)
    {
        const RasterRegion& region = regions[regionIndex];
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
                jobs.emplace_back(RasterRegion(region.GetLeft(), currentTop, region.GetRight(), bottom));
                currentTop = bottom;
            }
        }
    }

    while (jobs.size() < threadNumber)
        jobs.emplace_back();

    return jobs;
}

void RasterFractal::Resize(const unsigned int width, const unsigned int height)
{
    // Stop threads if they are still rendering.
    this->StopRender();
    _paused = false;

    ReleaseRenderMaps();

    // Copy window properties.
    _screenHeight = height;
    _screenWidth = width;
    UpdateRenderDimensions();
    AllocateRenderMaps();
    ClearRenderMaps(InvalidColor);

    EnsurePreciseViewInitialized();
    _preciseView.top = _preciseView.bottom + (_preciseView.right - _preciseView.left) *
        HighPrecisionReal(_screenHeight) / HighPrecisionReal(_screenWidth);
    UpdatePreciseFactors();
    SyncDoubleViewFromPrecise();

}

void RasterFractal::PrepareRender(const Vector2Int reusedMapOffset)
{
    this->PreRender();
    _pendingRenderOffset = DisplayOffsetToRenderOffset(reusedMapOffset);
    NotifyPrecisionStatusIfChanged();

    // Checks if the movement is valid.
    if ((abs(_pendingRenderOffset.x) >= _renderWidth) || (abs(_pendingRenderOffset.y) >= _renderHeight))
        _redrawAll = true;

    // Clear maps.
    if ((!_pendingRenderOffset.x && !_pendingRenderOffset.y) || _redrawAll || _redrawAlways)
    {
        const double initialColorValue = _algorithm == RenderingAlgorithmType::Buddhabrot ? 0.0 : InvalidColor;
        ClearRenderMaps(initialColorValue);
        _pendingRenderOffset = {0, 0};
        _redrawAll = false;
    }
}

void RasterFractal::ReuseRenderedMaps(const Vector2Int reusedMapOffset)
{
    const Vector2Int renderOffset = DisplayOffsetToRenderOffset(reusedMapOffset);
    MoveMatrix<bool>(_setMap, _renderHeight, _renderWidth, renderOffset.y, renderOffset.x);
    MoveMatrix<double>(_colorMap, _renderHeight, _renderWidth, renderOffset.y, renderOffset.x, InvalidColor);
}

void RasterFractal::PrepareDisplayColorLookup()
{
    UpdateMaxColorMapValue();
}

double RasterFractal::NormalizeColorMapValue(const double value) const
{
    const double minValue = _relativeColor ? _relativeColorMin : 0.0;
    const double maxValue = _relativeColor ? _relativeColorMax : static_cast<double>(_maxIterations);
    return PaletteMapping::Map(value, minValue, maxValue, _paletteSize, _colorCycleLength,
                               _paletteMappingMode, _paletteMappingExponent, _relativeColor);
}

bool RasterFractal::HasDisplayPixelColor(const unsigned int x, const unsigned int y) const
{
    if (x >= _screenWidth || y >= _screenHeight)
        return false;

    if (_antiAliasingScale > 1)
    {
        const unsigned int renderX = x * _antiAliasingScale;
        const unsigned int renderY = y * _antiAliasingScale;
        for (unsigned int offsetX = 0; offsetX < _antiAliasingScale; offsetX++)
        {
            for (unsigned int offsetY = 0; offsetY < _antiAliasingScale; offsetY++)
            {
                if (HasRenderMapPixelColor(renderX + offsetX, renderY + offsetY))
                    return true;
            }
        }

        return false;
    }

    return HasRenderMapPixelColor(x, y);
}

bool RasterFractal::HasRenderMapPixelColor(const unsigned int x, const unsigned int y) const
{
    if (x >= _renderWidth || y >= _renderHeight)
        return false;

    if (_setMap[x][y] && _colorSet)
        return true;

    return _colorMode && IsValidColorMapValue(_colorMap[x][y]);
}

sf::Color RasterFractal::GetRenderMapPixelColor(const unsigned int x, const unsigned int y) const
{
    if (_setMap[x][y] && _colorSet)
        return GetSetColor();

    if (!_colorMode || !IsValidColorMapValue(_colorMap[x][y]))
        return sf::Color::White;

    return GetColorFromPalette(NormalizeColorMapValue(_colorMap[x][y]) + _changeGradient);
}

void RasterFractal::SetAntiAliasingScale(const unsigned int scale)
{
    const unsigned int normalizedScale = NormalizeAntiAliasingScale(scale);
    if (_antiAliasingScale == normalizedScale)
        return;

    this->StopRender();
    ReleaseRenderMaps();
    _antiAliasingScale = normalizedScale;
    UpdateRenderDimensions();
    AllocateRenderMaps();
    const double initialColorValue = _algorithm == RenderingAlgorithmType::Buddhabrot ? 0.0 : InvalidColor;
    ClearRenderMaps(initialColorValue);
    _pendingRenderOffset = {0, 0};
    _redrawAll = true;
    _rendered = false;
    _rendering = false;
    _paused = false;
    _refreshImage = true;
}

void RasterFractal::RefreshAnimatedColors(sf::Image& image)
{
    /* This is admittedly an awful and inefficient algorithm, but the alternative is to use a GPU shader and redesign
     * from scratch the rendering pipeline. So I'm keeping this unless anyone complains. */

    UpdateMaxColorMapValue();

    const unsigned int workerCount = std::min(_threadNumber, _screenWidth);
    constexpr unsigned int minPixelsForParallelRefresh = 120000;
    const bool useParallelRefresh = workerCount > 1 && _screenWidth * _screenHeight >= minPixelsForParallelRefresh;

    const auto refreshColumns = [this, &image](const unsigned int beginX, const unsigned int endX)
    {
        for (unsigned int i = beginX; i < endX; i++)
        {
            for (unsigned int j = 0; j < _screenHeight; j++)
            {
                if (HasDisplayPixelColor(i, j))
                    image.setPixel(i, j, GetRenderedPixelColor(i, j));
            }
        }
    };

    if (!useParallelRefresh)
    {
        refreshColumns(0, _screenWidth);
        return;
    }

    std::vector<std::thread> workers;
    workers.reserve(workerCount - 1);
    const unsigned int columnsPerWorker = _screenWidth / workerCount;
    unsigned int beginX = 0;

    for (unsigned int workerIndex = 1; workerIndex < workerCount; workerIndex++)
    {
        const unsigned int endX = beginX + columnsPerWorker;
        workers.emplace_back(refreshColumns, beginX, endX);
        beginX = endX;
    }

    refreshColumns(beginX, _screenWidth);

    for (std::thread& worker : workers)
        worker.join();
}

void RasterFractal::ClearOrbitLines()
{
    _orbitLines.clear();
}

Fractal::PointSample RasterFractal::GetPointSample(const unsigned int x, const unsigned int y) const
{
    if (x >= _screenWidth || y >= _screenHeight)
        return {false, 0, false};

    const unsigned int scale = _antiAliasingScale;
    const unsigned int renderX = x * scale + scale / 2;
    const unsigned int renderY = y * scale + scale / 2;
    return {_setMap[renderX][renderY], _colorMap[renderX][renderY], IsValidColorMapValue(_colorMap[renderX][renderY])};
}

int RasterFractal::GetRenderProgress() const
{
    return _renderPool.GetProgress();
}

void RasterFractal::PauseContinue()
{
    if (_paused)
    {
        this->PreRestartRender();
        _rendered = false;
        _rendering = true;
        this->Render();
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

bool RasterFractal::StopRender()
{
    if (this->IsRendering())
    {
        _renderPool.Stop();
        _rendering = false;
        return true;
    }
    return false;
}

bool RasterFractal::IsRendering()
{
    if (_waitRoutine)
        return false;
    return _renderPool.IsRunning();
}

sf::Color RasterFractal::GetRenderedPixelColor(const unsigned int x, const unsigned int y) const
{
    if (x >= _screenWidth || y >= _screenHeight)
        return sf::Color::White;

    if (_antiAliasingScale == 1)
        return GetRenderMapPixelColor(x, y);

    unsigned int red = 0;
    unsigned int green = 0;
    unsigned int blue = 0;
    unsigned int alpha = 0;
    unsigned int samples = 0;
    const unsigned int renderX = x * _antiAliasingScale;
    const unsigned int renderY = y * _antiAliasingScale;

    for (unsigned int offsetX = 0; offsetX < _antiAliasingScale; offsetX++)
    {
        for (unsigned int offsetY = 0; offsetY < _antiAliasingScale; offsetY++)
        {
            const unsigned int sampleX = renderX + offsetX;
            const unsigned int sampleY = renderY + offsetY;
            if (!HasRenderMapPixelColor(sampleX, sampleY))
                continue;

            const sf::Color color = GetRenderMapPixelColor(sampleX, sampleY);
            red += color.r;
            green += color.g;
            blue += color.b;
            alpha += color.a;
            samples++;
        }
    }

    if (samples == 0)
        return sf::Color::White;

    return {static_cast<sf::Uint8>(red / samples),
            static_cast<sf::Uint8>(green / samples),
            static_cast<sf::Uint8>(blue / samples),
            static_cast<sf::Uint8>(alpha / samples)};
}

sf::Image RasterFractal::GetRenderedImage()
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

bool RasterFractal::SaveBmp(const string& filename)
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

void RasterFractal::DrawLine(const double x1, const double y1, const double x2, const double y2, const sf::Color color,
                             const bool orbitLine)
{
    if (!orbitLine)
    {
        Fractal::DrawLine(x1, y1, x2, y2, color, false);
        return;
    }

    _orbitLines.push_back({x1, y1, x2, y2, color});
    _geomFigure = true;
}
