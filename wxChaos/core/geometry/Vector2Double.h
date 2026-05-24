#pragma once
#include "Vector2Int.h"

struct Vector2Double
{
    double _x, _y;

    Vector2Double();
    Vector2Double(double x, double y);
    explicit Vector2Double(const Vector2Int& v);

    Vector2Double operator-() const;
    Vector2Double& operator+=(const Vector2Double& v);

    Vector2Double& operator*=(double t);
    Vector2Double& operator/=(double t);

    double Length() const;
    double SquaredLength() const;
};

inline Vector2Double operator+(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u._x + v._x, u._y + v._y);
}
inline Vector2Double operator-(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u._x - v._x, u._y - v._y);
}
inline Vector2Double operator*(const Vector2Double& u, const Vector2Double& v)
{
    return Vector2Double(u._x * v._x, u._y * v._y);
}
inline Vector2Double operator*(double t, const Vector2Double& v)
{
    return Vector2Double(t * v._x, t * v._y);
}
inline Vector2Double operator*(const Vector2Double& v, double t)
{
    return t * v;
}
inline Vector2Double operator/(Vector2Double v, double t)
{
    return (1.0 / t) * v;
}
