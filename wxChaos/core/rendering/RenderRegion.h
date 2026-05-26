#pragma once
#ifndef RENDER_REGION_H
#define RENDER_REGION_H

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

    int GetLeft() const;
    int GetTop() const;
    int GetRight() const;
    int GetBottom() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetArea() const;
    bool IsEmpty() const;
};

#endif
