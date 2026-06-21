#include <doctest/doctest.h>
#include "geometry/Rect.h"
#include "geometry/Vector2Double.h"
#include "geometry/Vector2Int.h"

TEST_CASE("Vector2Double defaults to zero")
{
    const Vector2Double value;

    CHECK(value._x == doctest::Approx(0.0));
    CHECK(value._y == doctest::Approx(0.0));
    CHECK(value.Length() == doctest::Approx(0.0));
    CHECK(value.SquaredLength() == doctest::Approx(0.0));
}

TEST_CASE("Vector2Double supports arithmetic")
{
    Vector2Double value(3.0, 4.0);

    CHECK(value.Length() == doctest::Approx(5.0));
    CHECK(value.SquaredLength() == doctest::Approx(25.0));

    value += Vector2Double(1.5, -2.0);
    CHECK(value._x == doctest::Approx(4.5));
    CHECK(value._y == doctest::Approx(2.0));

    value *= 2.0;
    CHECK(value._x == doctest::Approx(9.0));
    CHECK(value._y == doctest::Approx(4.0));

    value /= 4.0;
    CHECK(value._x == doctest::Approx(2.25));
    CHECK(value._y == doctest::Approx(1.0));

    const Vector2Double negated = -value;
    CHECK(negated._x == doctest::Approx(-2.25));
    CHECK(negated._y == doctest::Approx(-1.0));
}

TEST_CASE("Vector2Double free operators return component-wise results")
{
    const Vector2Double left(6.0, -3.0);
    const Vector2Double right(2.0, 5.0);

    const Vector2Double sum = left + right;
    CHECK(sum._x == doctest::Approx(8.0));
    CHECK(sum._y == doctest::Approx(2.0));

    const Vector2Double difference = left - right;
    CHECK(difference._x == doctest::Approx(4.0));
    CHECK(difference._y == doctest::Approx(-8.0));

    const Vector2Double product = left * right;
    CHECK(product._x == doctest::Approx(12.0));
    CHECK(product._y == doctest::Approx(-15.0));

    const Vector2Double scaled = 3.0 * right;
    CHECK(scaled._x == doctest::Approx(6.0));
    CHECK(scaled._y == doctest::Approx(15.0));

    const Vector2Double divided = left / 2.0;
    CHECK(divided._x == doctest::Approx(3.0));
    CHECK(divided._y == doctest::Approx(-1.5));
}

TEST_CASE("Vector2Double converts from Vector2Int")
{
    const Vector2Double value(Vector2Int{7, -9});

    CHECK(value._x == doctest::Approx(7.0));
    CHECK(value._y == doctest::Approx(-9.0));
}

TEST_CASE("Rect stores and updates lower and upper bounds")
{
    Rect view(-2.0, -1.0, 3.0, 4.0);

    CHECK(view.GetLowerBound()._x == doctest::Approx(-2.0));
    CHECK(view.GetLowerBound()._y == doctest::Approx(-1.0));
    CHECK(view.GetUpperBound()._x == doctest::Approx(3.0));
    CHECK(view.GetUpperBound()._y == doctest::Approx(4.0));

    view.SetLowerBound(Vector2Double(-5.0, -6.0));
    view.SetUpperBound(Vector2Double(7.0, 8.0));

    CHECK(view._left == doctest::Approx(-5.0));
    CHECK(view._bottom == doctest::Approx(-6.0));
    CHECK(view._right == doctest::Approx(7.0));
    CHECK(view._top == doctest::Approx(8.0));
}
