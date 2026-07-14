#pragma once

/**
* @class RasterRegion
* @brief Represents a rectangular pixel area that needs to be rendered.
*
* RasterRegion stores the half-open bounds [left, right) and [top, bottom)
* passed to Renderer::SetLimits.
*/
class RasterRegion
{
    int _left;
    int _top;
    int _right;
    int _bottom;

public:
    RasterRegion();
    RasterRegion(int left, int top, int right, int bottom);

    [[nodiscard]] int GetLeft() const;
    [[nodiscard]] int GetTop() const;
    [[nodiscard]] int GetRight() const;
    [[nodiscard]] int GetBottom() const;
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] int GetArea() const;
    [[nodiscard]] bool IsEmpty() const;
};
