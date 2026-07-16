#pragma once

#include <cstdint>
#include <vector>

class Fractal;
struct CircleData;
struct LineData;
struct RectangleData;
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
    int _size{};                            ///< Width and height of the square occupancy map in pixels.
    std::vector<std::uint8_t> _occupied;   ///< Row-major occupancy flags; nonzero values represent the fractal set.

    /** @brief Changes a pixel's occupancy when it lies inside the map bounds. */
    void SetOccupied(int x, int y, bool occupied = true);

    /** @brief Copies set-membership pixels from a raster fractal map. */
    void CopyRasterMap(bool** map);

    /** @brief Rasterizes every published primitive belonging to a vector fractal. */
    void RasterizeVectorGeometry(const Fractal& fractal);

    /** @brief Clips and rasterizes one world-coordinate line. */
    void RasterizeLine(const LineData& line, const Rect& view);

    /** @brief Draws a pixel-coordinate line into the occupancy map with Bresenham's algorithm. */
    void RasterizeLinePixels(double x1, double y1, double x2, double y2);

    /** @brief Rasterizes the outline or filled area of one world-coordinate circle. */
    void RasterizeCircle(const CircleData& circle, const Rect& view);

    /** @brief Fills or clears the pixels covered by one world-coordinate rectangle. */
    void RasterizeRectangle(const RectangleData& rectangle, const Rect& view);

    /**
     * @brief Clips one coordinate axis of a line segment to an interval.
     * @return true when some portion of the segment remains inside the interval.
     */
    static bool ClipLine(double& x1, double& y1, double& x2, double& y2, double minimum, double maximum);

    /**
     * @brief Updates the entering and exiting parameters used by line clipping.
     * @return false when the segment lies completely outside the clipping boundary.
     */
    static bool ClipDirection(double direction, double distance, double& entry, double& exit);

    /** @brief Converts a finite pixel coordinate to a valid map index. */
    [[nodiscard]] int ClampPixel(double coordinate) const;

public:
    /**
     * @brief Rebuilds this map from the completed output of a fractal.
     * @param fractal Raster or vector fractal whose visible set should be represented.
     */
    void Build(const Fractal& fractal);

    /**
     * @brief Tests whether a pixel belongs to the represented fractal set.
     * @return true for occupied in-bounds pixels; false for empty or out-of-bounds pixels.
     */
    [[nodiscard]] bool IsOccupied(int x, int y) const;

    /** @brief Returns the width and height of the square map in pixels. */
    [[nodiscard]] int GetSize() const { return _size; }

    /** @brief Returns the total number of occupied pixels in the map. */
    [[nodiscard]] std::size_t GetOccupiedPixelCount() const;
};
