#include "analysis/BoxCountMap.h"
#include <algorithm>
#include <cmath>
#include "Fractal.h"

void BoxCountMap::Build(const Fractal& fractal)
{
    const sf::Vector2u screenSize = fractal.GetScreenSize();
    _size = static_cast<int>(std::min(screenSize.x, screenSize.y));
    _occupied.assign(static_cast<std::size_t>(_size) * _size, 0);

    if (_size == 0)
        return;

    if (fractal.IsVectorFractal())
        RasterizeVectorGeometry(fractal);
    else
        CopyRasterMap(fractal.GetSetMap());
}

bool BoxCountMap::IsOccupied(const int x, const int y) const
{
    if (x < 0 || y < 0 || x >= _size || y >= _size)
        return false;
    return _occupied[static_cast<std::size_t>(y) * _size + x] != 0;
}

std::size_t BoxCountMap::GetOccupiedPixelCount() const
{
    return static_cast<std::size_t>(std::count(_occupied.begin(), _occupied.end(), std::uint8_t{1}));
}

void BoxCountMap::SetOccupied(const int x, const int y, const bool occupied)
{
    if (x >= 0 && y >= 0 && x < _size && y < _size)
        _occupied[static_cast<std::size_t>(y) * _size + x] = occupied ? 1 : 0;
}

void BoxCountMap::CopyRasterMap(bool** map)
{
    if (map == nullptr)
        return;

    for (int x = 0; x < _size; x++)
    {
        for (int y = 0; y < _size; y++)
        {
            if (map[x][y])
                SetOccupied(x, y);
        }
    }
}

void BoxCountMap::RasterizeVectorGeometry(const Fractal& fractal)
{
    const Rect view = fractal.GetView();
    for (const RectangleData& rectangle : fractal.GetRectangles())
        RasterizeRectangle(rectangle, view);

    for (const LineData& line : fractal.GetLines())
        RasterizeLine(line, view);

    for (const CircleData& circle : fractal.GetCircles())
        RasterizeCircle(circle, view);
}

void BoxCountMap::RasterizeRectangle(const RectangleData& rectangle, const Rect& view)
{
    const double viewWidth = view._right - view._left;
    const double viewHeight = view._top - view._bottom;
    if (viewWidth <= 0.0 || viewHeight <= 0.0 || rectangle.left >= rectangle.right
        || rectangle.bottom >= rectangle.top || rectangle.right < view._left || rectangle.left > view._right
        || rectangle.top < view._bottom || rectangle.bottom > view._top)
        return;

    const auto pixelMaximum = static_cast<double>(_size - 1);
    const double clippedLeft = std::max(rectangle.left, view._left);
    const double clippedRight = std::min(rectangle.right, view._right);
    const double clippedBottom = std::max(rectangle.bottom, view._bottom);
    const double clippedTop = std::min(rectangle.top, view._top);
    const int firstX = ClampPixel(std::ceil((clippedLeft - view._left) * pixelMaximum / viewWidth));
    const int lastX = ClampPixel(std::floor((clippedRight - view._left) * pixelMaximum / viewWidth));
    const int firstY = ClampPixel(std::ceil((view._top - clippedTop) * pixelMaximum / viewHeight));
    const int lastY = ClampPixel(std::floor((view._top - clippedBottom) * pixelMaximum / viewHeight));

    for (int y = firstY; y <= lastY; y++)
    {
        for (int x = firstX; x <= lastX; x++)
            SetOccupied(x, y, rectangle.belongsToSet);
    }
}

void BoxCountMap::RasterizeLine(const LineData& line, const Rect& view)
{
    double x1 = line.x1;
    double y1 = line.y1;
    double x2 = line.x2;
    double y2 = line.y2;
    if (!ClipLine(x1, y1, x2, y2, view._left, view._right)
        || !ClipLine(y1, x1, y2, x2, view._bottom, view._top))
        return;

    const double width = view._right - view._left;
    const double height = view._top - view._bottom;
    if (width <= 0.0 || height <= 0.0)
        return;

    const auto pixelMaximum = static_cast<double>(_size - 1);
    RasterizeLinePixels((x1 - view._left) * pixelMaximum / width,
                        (view._top - y1) * pixelMaximum / height,
                        (x2 - view._left) * pixelMaximum / width,
                        (view._top - y2) * pixelMaximum / height);
}

void BoxCountMap::RasterizeLinePixels(const double x1, const double y1, const double x2, const double y2)
{
    int x = ClampPixel(std::round(x1));
    int y = ClampPixel(std::round(y1));
    const int endX = ClampPixel(std::round(x2));
    const int endY = ClampPixel(std::round(y2));
    const int deltaX = std::abs(endX - x);
    const int stepX = x < endX ? 1 : -1;
    const int deltaY = -std::abs(endY - y);
    const int stepY = y < endY ? 1 : -1;
    int error = deltaX + deltaY;

    while (true)
    {
        SetOccupied(x, y);
        if (x == endX && y == endY)
            break;

        const int twiceError = 2 * error;
        if (twiceError >= deltaY)
        {
            error += deltaY;
            x += stepX;
        }
        if (twiceError <= deltaX)
        {
            error += deltaX;
            y += stepY;
        }
    }
}

void BoxCountMap::RasterizeCircle(const CircleData& circle, const Rect& view)
{
    const double width = view._right - view._left;
    const double height = view._top - view._bottom;
    if (width <= 0.0 || height <= 0.0 || circle.radius <= 0.0)
        return;

    const auto pixelMaximum = static_cast<double>(_size - 1);
    const double centerX = (circle.xCenter - view._left) * pixelMaximum / width;
    const double centerY = (view._top - circle.yCenter) * pixelMaximum / height;
    const double radiusX = std::abs(circle.radius * pixelMaximum / width);
    const double radiusY = std::abs(circle.radius * pixelMaximum / height);
    if (centerX + radiusX < 0.0 || centerX - radiusX > pixelMaximum
        || centerY + radiusY < 0.0 || centerY - radiusY > pixelMaximum)
        return;

    const int firstY = ClampPixel(std::ceil(centerY - radiusY));
    const int lastY = ClampPixel(std::floor(centerY + radiusY));
    for (int y = firstY; y <= lastY; y++)
    {
        const long double normalizedY = (static_cast<long double>(y) - centerY) / radiusY;
        if (std::abs(normalizedY) > 1.0L)
            continue;

        const double offsetX = radiusX * std::sqrt(static_cast<double>(1.0L - normalizedY * normalizedY));
        const int left = ClampPixel(std::round(centerX - offsetX));
        const int right = ClampPixel(std::round(centerX + offsetX));
        if (circle.filled)
        {
            for (int x = left; x <= right; x++)
                SetOccupied(x, y);
        }
        else
        {
            SetOccupied(left, y);
            SetOccupied(right, y);
        }
    }

    if (circle.filled)
        return;

    const int firstX = ClampPixel(std::ceil(centerX - radiusX));
    const int lastX = ClampPixel(std::floor(centerX + radiusX));
    for (int x = firstX; x <= lastX; x++)
    {
        const long double normalizedX = (static_cast<long double>(x) - centerX) / radiusX;
        if (std::abs(normalizedX) > 1.0L)
            continue;

        const double offsetY = radiusY * std::sqrt(static_cast<double>(1.0L - normalizedX * normalizedX));
        SetOccupied(x, ClampPixel(std::round(centerY - offsetY)));
        SetOccupied(x, ClampPixel(std::round(centerY + offsetY)));
    }
}

bool BoxCountMap::ClipLine(double& x1, double& y1, double& x2, double& y2, const double minimum,
                           const double maximum)
{
    const double delta = x2 - x1;
    double entry = 0.0;
    double exit = 1.0;
    if (!ClipDirection(-delta, x1 - minimum, entry, exit)
        || !ClipDirection(delta, maximum - x1, entry, exit))
        return false;

    const double originalX = x1;
    const double originalY = y1;
    x1 = originalX + entry * (x2 - originalX);
    y1 = originalY + entry * (y2 - originalY);
    x2 = originalX + exit * (x2 - originalX);
    y2 = originalY + exit * (y2 - originalY);
    return true;
}

bool BoxCountMap::ClipDirection(const double direction, const double distance, double& entry, double& exit)
{
    if (direction == 0.0)
        return distance >= 0.0;

    const double ratio = distance / direction;
    if (direction < 0.0)
    {
        if (ratio > exit)
            return false;
        entry = std::max(entry, ratio);
    }
    else
    {
        if (ratio < entry)
            return false;
        exit = std::min(exit, ratio);
    }
    return true;
}

int BoxCountMap::ClampPixel(const double coordinate) const
{
    if (!std::isfinite(coordinate) || coordinate <= 0.0)
        return 0;
    if (coordinate >= _size - 1)
        return _size - 1;
    return static_cast<int>(coordinate);
}
