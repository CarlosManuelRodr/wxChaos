#include <cmath>
#include "Vector2Double.h"
#include "Vector2Int.h"

Vector2Double::Vector2Double()
{
    _x = _y = 0.0;
}
Vector2Double::Vector2Double(const double x, const double y)
{
    _x = x;
    _y = y;
}
Vector2Double::Vector2Double(const Vector2Int& v)
{
    _x = static_cast<double>(v.x);
    _y = static_cast<double>(v.y);
}

Vector2Double Vector2Double::operator-() const
{
    return Vector2Double(-_x, -_y);
}
Vector2Double& Vector2Double::operator+=(const Vector2Double& v)
{
    _x += v._x;
    _y += v._y;
    return *this;
}
Vector2Double& Vector2Double::operator*=(const double t)
{
    _x *= t;
    _y *= t;
    return *this;
}
Vector2Double& Vector2Double::operator/=(const double t)
{
    return *this *= 1.0 / t;
}

double Vector2Double::Length() const
{
    return sqrt(this->SquaredLength());
}
double Vector2Double::SquaredLength() const
{
    return _x * _x + _y * _y;
}
