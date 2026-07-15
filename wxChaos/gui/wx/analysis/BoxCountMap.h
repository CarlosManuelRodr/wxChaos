#pragma once

#include <cstdint>
#include <vector>

class Fractal;
struct CircleData;
struct LineData;
struct Rect;

/**
 * @class BoxCountMap
 * @brief Calculator-owned pixel occupancy map used by box-counting analysis.
 *
 * Raster fractals copy their set membership into this map. Vector fractals
 * rasterize their published primitives into it without adding raster storage
 * to VectorFractal itself.
 */
class BoxCountMap
{
    int _size{};
    std::vector<std::uint8_t> _occupied;

    void SetOccupied(int x, int y);
    void CopyRasterMap(bool** map);
    void RasterizeVectorGeometry(const Fractal& fractal);
    void RasterizeLine(const LineData& line, const Rect& view);
    void RasterizeLinePixels(double x1, double y1, double x2, double y2);
    void RasterizeCircle(const CircleData& circle, const Rect& view);
    static bool ClipLine(double& x1, double& y1, double& x2, double& y2, double minimum, double maximum);
    static bool ClipDirection(double direction, double distance, double& entry, double& exit);
    [[nodiscard]] int ClampPixel(double coordinate) const;

public:
    void Build(const Fractal& fractal);
    [[nodiscard]] bool IsOccupied(int x, int y) const;
    [[nodiscard]] int GetSize() const { return _size; }
    [[nodiscard]] std::size_t GetOccupiedPixelCount() const;
};
