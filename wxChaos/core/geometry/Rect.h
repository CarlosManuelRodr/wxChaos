#pragma once
#include "Vector2Double.h"

struct Rect
{
    double left, bottom, right, top;

    Rect();
    Rect(double _left, double _bottom, double _right, double _top);
    Vector2Double GetLowerBound();
    Vector2Double GetHigherBound();
    void SetLowerBound(Vector2Double lb);
    void SetHigherBound(Vector2Double hb);
};
