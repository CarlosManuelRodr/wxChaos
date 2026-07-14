#pragma once

#include "Fractal.h"
#include "RenderWorker.h"
#include "rasterization/RasterJob.h"
#include "rasterization/RasterRegion.h"
#include "rasterization/RasterThreadPool.h"

/**
 * @class RasterFractal
 * @brief Base for fractals rendered into rectangular set and color maps.
 */
class RasterFractal : public Fractal
{
protected:
    bool** _setMap{};                ///< Points that belong to the fractal set.
    double** _colorMap{};            ///< Continuous values used by coloring algorithms.
    RasterThreadPool _renderPool;    ///< Reusable pool for raster render jobs.
    std::vector<LineData> _orbitLines;
    unsigned int _threadNumber;      ///< Number of raster render workers.
    unsigned int _renderWidth{};
    unsigned int _renderHeight{};
    unsigned int _backRenderWidth{};
    unsigned int _antiAliasingScale{1};
    Vector2Int _pendingRenderOffset{Vector2Int::Zero()};

    double NormalizeColorMapValue(double value) const;
    static bool IsValidColorMapValue(double value);
    static unsigned int NormalizeAntiAliasingScale(unsigned int scale);
    void UpdateRenderDimensions();
    void AllocateRenderMaps();
    void ReleaseRenderMaps();
    void ClearRenderMaps(double initialColorValue);
    [[nodiscard]] Vector2Int DisplayOffsetToRenderOffset(Vector2Int displayOffset) const;
    [[nodiscard]] HighPrecisionReal GetRenderPreciseXFactor() const;
    [[nodiscard]] HighPrecisionReal GetRenderPreciseYFactor() const;
    [[nodiscard]] Options GetRenderOptions() const;
    [[nodiscard]] bool HasRenderMapPixelColor(unsigned int x, unsigned int y) const;
    [[nodiscard]] sf::Color GetRenderMapPixelColor(unsigned int x, unsigned int y) const;
    void RedrawMaps() override;
    void UpdateMaxColorMapValue();
    void ConfigureRenderer(RenderWorker& renderer) const;
    std::vector<RasterRegion> BuildRenderRegions() const;
    std::vector<RasterJob> BuildRenderJobs(const std::vector<RasterRegion>& regions, int tileHeight) const;

    template<class M>
    void MoveMatrix(M** matrix, unsigned int matrixWidth, unsigned int matrixHeight, int moveX, int moveY,
                    M fillValue = M{});

public:
    RasterFractal(unsigned int width, unsigned int height);
    ~RasterFractal() override;

    void Resize(unsigned int width, unsigned int height) override;
    void PrepareRender(Vector2Int reusedMapOffset = {0, 0}) override;
    void ReuseRenderedMaps(Vector2Int reusedMapOffset) override;
    void PrepareDisplayColorLookup() override;
    bool HasDisplayPixelColor(unsigned int x, unsigned int y) const override;
    sf::Color GetRenderedPixelColor(unsigned int x, unsigned int y) const override;
    bool SupportsAntiAliasing() const override { return true; }
    void SetAntiAliasingScale(unsigned int scale) override;
    unsigned int GetAntiAliasingScale() const override { return _antiAliasingScale; }
    void RefreshAnimatedColors(sf::Image& image) override;
    void ClearOrbitLines() override;
    const std::vector<LineData>& GetOrbitLines() const override { return _orbitLines; }
    PointSample GetPointSample(unsigned int x, unsigned int y) const override;
    int GetRenderProgress() const override;
    void PauseContinue() override;
    bool StopRender() override;
    bool IsRendering() override;
    bool** GetSetMap() const override { return _setMap; }
    sf::Image GetRenderedImage() override;
    bool SaveBmp(const std::string& filename) override;
    void DrawLine(double x1, double y1, double x2, double y2, sf::Color color = sf::Color(0, 0, 0),
                  bool orbitLine = false) override;

    template<class DerivedRenderer>
    void SetRendererBounds(DerivedRenderer* renderers, int tileHeight = 16);
};

template<class DerivedRenderer>
void RasterFractal::SetRendererBounds(DerivedRenderer* renderers, const int tileHeight)
{
    const std::vector<RasterRegion> regions = BuildRenderRegions();
    const int renderTileHeight = tileHeight > 0 ? tileHeight * static_cast<int>(_antiAliasingScale) : tileHeight;
    const std::vector<RasterJob> jobs = BuildRenderJobs(regions, renderTileHeight);
    _pendingRenderOffset = {0, 0};

    std::vector<RenderWorker*> workers;
    workers.reserve(_threadNumber);
    for (unsigned int i = 0; i < _threadNumber; i++)
    {
        ConfigureRenderer(renderers[i]);
        workers.push_back(&renderers[i]);
    }

    _renderPool.Render(workers, jobs);
    if (_waitRoutine)
        _renderPool.Wait();
}

template<class M>
void RasterFractal::MoveMatrix(M** matrix, const unsigned int matrixWidth, const unsigned int matrixHeight,
                               const int moveX, const int moveY, const M fillValue)
{
    if (matrix == nullptr || matrixWidth == 0 || matrixHeight == 0)
        return;

    if (std::abs(moveX) >= static_cast<int>(matrixWidth) || std::abs(moveY) >= static_cast<int>(matrixHeight))
    {
        for (unsigned int i = 0; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
        return;
    }

    if (moveX > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move_backward(matrix[i], matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth);
            std::fill(matrix[i], matrix[i] + displacement, fillValue);
        }
    }
    else if (moveX < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveX);
        for (unsigned int i = 0; i < matrixHeight; i++)
        {
            std::move(matrix[i] + displacement, matrix[i] + matrixWidth, matrix[i]);
            std::fill(matrix[i] + matrixWidth - displacement, matrix[i] + matrixWidth, fillValue);
        }
    }

    if (moveY > 0)
    {
        const auto displacement = static_cast<unsigned int>(moveY);
        std::rotate(matrix, matrix + matrixHeight - displacement, matrix + matrixHeight);
        for (unsigned int i = 0; i < displacement; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
    else if (moveY < 0)
    {
        const auto displacement = static_cast<unsigned int>(-moveY);
        std::rotate(matrix, matrix + displacement, matrix + matrixHeight);
        for (unsigned int i = matrixHeight - displacement; i < matrixHeight; i++)
            std::fill(matrix[i], matrix[i] + matrixWidth, fillValue);
    }
}
