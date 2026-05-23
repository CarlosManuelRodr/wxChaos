#include "Rect.h"

Rect::Rect()
{
    _left = _top = _right = _bottom = 0.0;
}
Rect::Rect(const double left, const double bottom, const double right, const double top)
{
    _left = left;
    _bottom = bottom;
    _right = right;
    _top = top;
}
Vector2Double Rect::GetLowerBound() const
{
    return Vector2Double(_left, _bottom);
}
Vector2Double Rect::GetUpperBound() const
{
    return Vector2Double(_right, _top);
}
void Rect::SetLowerBound(const Vector2Double lb)
{
    _left = lb._x;
    _bottom = lb._y;
}
void Rect::SetUpperBound(const Vector2Double hb)
{
    _right = hb._x;
    _top = hb._y;
}
