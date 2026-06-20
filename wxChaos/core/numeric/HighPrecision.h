#pragma once

#include <cmath>
#include <type_traits>
#include <boost/multiprecision/cpp_dec_float.hpp>

using HighPrecisionReal = boost::multiprecision::cpp_dec_float_100;

template<class Real>
double ToDouble(const Real& value)
{
    if constexpr (std::is_same_v<Real, double>)
        return value;
    else
    {
        const HighPrecisionReal evaluated(value);
        return evaluated.convert_to<double>();
    }
}

template<class Real>
auto RealAbs(const Real& value)
{
    using std::abs;
    return abs(value);
}

template<class Real>
struct PrecisionComplex
{
    Real re;
    Real im;

    PrecisionComplex() : re(0), im(0) {}
    PrecisionComplex(const Real& real, const Real& imaginary) : re(real), im(imaginary) {}
};

template<class Real>
PrecisionComplex<Real> operator+(const PrecisionComplex<Real>& left, const PrecisionComplex<Real>& right)
{
    return {left.re + right.re, left.im + right.im};
}

template<class Real>
PrecisionComplex<Real> operator-(const PrecisionComplex<Real>& left, const PrecisionComplex<Real>& right)
{
    return {left.re - right.re, left.im - right.im};
}

template<class Real>
PrecisionComplex<Real> operator*(const PrecisionComplex<Real>& left, const PrecisionComplex<Real>& right)
{
    return {left.re * right.re - left.im * right.im, left.re * right.im + left.im * right.re};
}

template<class Real>
PrecisionComplex<Real> operator*(const Real& value, const PrecisionComplex<Real>& z)
{
    return {value * z.re, value * z.im};
}

template<class Real>
PrecisionComplex<Real> operator*(const PrecisionComplex<Real>& z, const Real& value)
{
    return value * z;
}

template<class Real>
PrecisionComplex<Real> operator/(const PrecisionComplex<Real>& left, const PrecisionComplex<Real>& right)
{
    const Real denominator = right.re * right.re + right.im * right.im;
    return {
        (left.re * right.re + left.im * right.im) / denominator,
        (left.im * right.re - left.re * right.im) / denominator
    };
}

template<class Real>
PrecisionComplex<Real>& operator/=(PrecisionComplex<Real>& left, const PrecisionComplex<Real>& right)
{
    left = left / right;
    return left;
}

template<class Real>
Real ComplexNorm(const PrecisionComplex<Real>& z)
{
    return z.re * z.re + z.im * z.im;
}

template<class Real>
PrecisionComplex<Real> ComplexSin(const PrecisionComplex<Real>& z)
{
    using std::cos;
    using std::cosh;
    using std::sin;
    using std::sinh;
    return {sin(z.re) * cosh(z.im), cos(z.re) * sinh(z.im)};
}

template<class Real>
PrecisionComplex<Real> ComplexPow(PrecisionComplex<Real> z, int exponent)
{
    if (exponent == 0)
        return {Real(1), Real(0)};

    if (exponent < 0)
        return PrecisionComplex<Real>{Real(1), Real(0)} / ComplexPow(z, -exponent);

    PrecisionComplex<Real> result{Real(1), Real(0)};
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
            result = result * z;

        z = z * z;
        exponent /= 2;
    }

    return result;
}

template<class Real>
PrecisionComplex<Real> ComplexPow(const PrecisionComplex<Real>& z, const double exponent)
{
    using std::atan2;
    using std::cos;
    using std::pow;
    using std::sin;
    using std::sqrt;

    const Real radius = sqrt(ComplexNorm(z));
    const Real theta = atan2(z.im, z.re);
    const Real magnitude = pow(radius, Real(exponent));
    const Real angle = theta * Real(exponent);
    return {magnitude * cos(angle), magnitude * sin(angle)};
}
