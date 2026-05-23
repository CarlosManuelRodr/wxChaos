#pragma once
#include "Vector2Double.h"

struct Rect
{
    double _left, _bottom, _right, _top;

    Rect();
    Rect(double left, double bottom, double right, double top);
    Vector2Double GetLowerBound() const;
    Vector2Double GetUpperBound() const;
    void SetLowerBound(Vector2Double lb);
    void SetUpperBound(Vector2Double hb);
};
