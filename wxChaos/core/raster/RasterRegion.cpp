#include "raster/RasterRegion.h"

RasterRegion::RasterRegion()
{
    _left = 0;
    _top = 0;
    _right = 0;
    _bottom = 0;
}

RasterRegion::RasterRegion(const int left, const int top, const int right, const int bottom)
{
    _left = left;
    _top = top;
    _right = right;
    _bottom = bottom;
}

int RasterRegion::GetLeft() const
{
    return _left;
}

int RasterRegion::GetTop() const
{
    return _top;
}

int RasterRegion::GetRight() const
{
    return _right;
}

int RasterRegion::GetBottom() const
{
    return _bottom;
}

int RasterRegion::GetWidth() const
{
    return _right - _left;
}

int RasterRegion::GetHeight() const
{
    return _bottom - _top;
}

int RasterRegion::GetArea() const
{
    if (this->IsEmpty())
        return 0;

    return this->GetWidth() * this->GetHeight();
}

bool RasterRegion::IsEmpty() const
{
    return _right <= _left || _bottom <= _top;
}
