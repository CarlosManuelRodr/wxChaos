#include "Rect.h"

Rect::Rect()
{
    left = top = right = bottom = 0.0;
}
Rect::Rect(double _left, double _bottom, double _right, double _top)
{
    left = _left;
    bottom = _bottom;
    right = _right;
    top = _top;
}
Vector2Double Rect::GetLowerBound()
{
    return Vector2Double(left, bottom);
}
Vector2Double Rect::GetHigherBound()
{
    return Vector2Double(right, top);
}
void Rect::SetLowerBound(Vector2Double lb)
{
    left = lb.x;
    bottom = lb.y;
}
void Rect::SetHigherBound(Vector2Double hb)
{
    right = hb.x;
    top = hb.y;
}
