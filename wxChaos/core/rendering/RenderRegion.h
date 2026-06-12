#pragma once

/**
* @class RenderRegion
* @brief Represents a rectangular pixel area that needs to be rendered.
*
* RenderRegion stores the half-open bounds [left, right) and [top, bottom)
* used by RenderFractal::SetLimits.
*/
class RenderRegion
{
    int _left;
    int _top;
    int _right;
    int _bottom;

public:
    RenderRegion();
    RenderRegion(int left, int top, int right, int bottom);

    [[nodiscard]] int GetLeft() const;
    [[nodiscard]] int GetTop() const;
    [[nodiscard]] int GetRight() const;
    [[nodiscard]] int GetBottom() const;
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] int GetArea() const;
    [[nodiscard]] bool IsEmpty() const;
};
