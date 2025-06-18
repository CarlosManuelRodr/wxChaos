#include "base/Vector2Double.h"
#include "base/Vector2Int.h"

Vector2Double::Vector2Double()
{
    x = y = 0.0;
}
Vector2Double::Vector2Double(double _x, double _y)
{
    x = _x;
    y = _y;
}
Vector2Double::Vector2Double(const Vector2Int& v)
{
    x = static_cast<double>(v.x);
    y = static_cast<double>(v.y);
}

Vector2Double Vector2Double::operator-() const
{
    return Vector2Double(-x, -y);
}
Vector2Double& Vector2Double::operator+=(const Vector2Double& v)
{
    x += v.x;
    y += v.y;
    return *this;
}
Vector2Double& Vector2Double::operator*=(const double t)
{
    x *= t;
    y *= t;
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
    return x * x + y * y;
}
