#include "rendering/RenderRegion.h"

RenderRegion::RenderRegion()
{
    _left = 0;
    _top = 0;
    _right = 0;
    _bottom = 0;
}

RenderRegion::RenderRegion(const int left, const int top, const int right, const int bottom)
{
    _left = left;
    _top = top;
    _right = right;
    _bottom = bottom;
}

int RenderRegion::GetLeft() const
{
    return _left;
}

int RenderRegion::GetTop() const
{
    return _top;
}

int RenderRegion::GetRight() const
{
    return _right;
}

int RenderRegion::GetBottom() const
{
    return _bottom;
}

int RenderRegion::GetWidth() const
{
    return _right - _left;
}

int RenderRegion::GetHeight() const
{
    return _bottom - _top;
}

int RenderRegion::GetArea() const
{
    if (this->IsEmpty())
        return 0;

    return this->GetWidth() * this->GetHeight();
}

bool RenderRegion::IsEmpty() const
{
    return _right <= _left || _bottom <= _top;
}
