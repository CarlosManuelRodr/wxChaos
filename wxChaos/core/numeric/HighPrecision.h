#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <type_traits>
#include <mpfr.h>

class HighPrecisionReal
{
public:
    class PrecisionScope
    {
    public:
        explicit PrecisionScope(const unsigned int precisionBits)
            : _previousPrecision(GetDefaultPrecision())
        {
            SetDefaultPrecision(precisionBits);
        }

        ~PrecisionScope()
        {
            SetDefaultPrecision(_previousPrecision);
        }

        PrecisionScope(const PrecisionScope&) = delete;
        PrecisionScope& operator=(const PrecisionScope&) = delete;

    private:
        mpfr_prec_t _previousPrecision;
    };

    HighPrecisionReal()
    {
        mpfr_init2(_value, GetDefaultPrecision());
        mpfr_set_zero(_value, 0);
    }

    HighPrecisionReal(const int value)
    {
        mpfr_init2(_value, GetDefaultPrecision());
        mpfr_set_si(_value, value, kRoundingMode);
    }

    HighPrecisionReal(const unsigned int value)
    {
        mpfr_init2(_value, GetDefaultPrecision());
        mpfr_set_ui(_value, value, kRoundingMode);
    }

    HighPrecisionReal(const double value)
    {
        mpfr_init2(_value, GetDefaultPrecision());
        mpfr_set_d(_value, value, kRoundingMode);
    }

    HighPrecisionReal(const HighPrecisionReal& other)
    {
        mpfr_init2(_value, mpfr_get_prec(other._value));
        mpfr_set(_value, other._value, kRoundingMode);
    }

    HighPrecisionReal(HighPrecisionReal&& other) noexcept
    {
        mpfr_init2(_value, mpfr_get_prec(other._value));
        mpfr_swap(_value, other._value);
    }

    ~HighPrecisionReal()
    {
        mpfr_clear(_value);
    }

    HighPrecisionReal& operator=(const HighPrecisionReal& other)
    {
        if (this == &other)
            return *this;

        mpfr_set_prec(_value, mpfr_get_prec(other._value));
        mpfr_set(_value, other._value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator=(HighPrecisionReal&& other) noexcept
    {
        if (this == &other)
            return *this;

        mpfr_swap(_value, other._value);
        return *this;
    }

    HighPrecisionReal& operator=(const int value)
    {
        mpfr_set_si(_value, value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator=(const unsigned int value)
    {
        mpfr_set_ui(_value, value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator=(const double value)
    {
        mpfr_set_d(_value, value, kRoundingMode);
        return *this;
    }

    [[nodiscard]] double ToDouble() const
    {
        return mpfr_get_d(_value, kRoundingMode);
    }

    [[nodiscard]] mpfr_prec_t GetPrecision() const
    {
        return mpfr_get_prec(_value);
    }

    [[nodiscard]] std::string ToString(const unsigned int significantDigits) const
    {
        char* buffer = nullptr;
        const int digits = static_cast<int>(std::max(1U, significantDigits));
        if (mpfr_asprintf(&buffer, "%.*Rg", digits, _value) < 0 || buffer == nullptr)
            return {};

        std::string output(buffer);
        mpfr_free_str(buffer);
        return output;
    }

    [[nodiscard]] static HighPrecisionReal WithCurrentPrecision(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_set(result._value, value._value, kRoundingMode);
        return result;
    }

    static void SetDefaultPrecision(const unsigned int precisionBits)
    {
        _defaultPrecision = precisionBits < MPFR_PREC_MIN ? MPFR_PREC_MIN : precisionBits;
    }

    [[nodiscard]] static mpfr_prec_t GetDefaultPrecision()
    {
        return _defaultPrecision;
    }

    HighPrecisionReal& operator+=(const HighPrecisionReal& right)
    {
        mpfr_add(_value, _value, right._value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator-=(const HighPrecisionReal& right)
    {
        mpfr_sub(_value, _value, right._value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator*=(const HighPrecisionReal& right)
    {
        mpfr_mul(_value, _value, right._value, kRoundingMode);
        return *this;
    }

    HighPrecisionReal& operator/=(const HighPrecisionReal& right)
    {
        mpfr_div(_value, _value, right._value, kRoundingMode);
        return *this;
    }

    friend HighPrecisionReal operator+(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        HighPrecisionReal result;
        mpfr_add(result._value, left._value, right._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal operator-(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        HighPrecisionReal result;
        mpfr_sub(result._value, left._value, right._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal operator*(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        HighPrecisionReal result;
        mpfr_mul(result._value, left._value, right._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal operator/(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        HighPrecisionReal result;
        mpfr_div(result._value, left._value, right._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal operator-(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_neg(result._value, value._value, kRoundingMode);
        return result;
    }

    friend bool operator<(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return mpfr_less_p(left._value, right._value) != 0;
    }

    friend bool operator<=(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return mpfr_lessequal_p(left._value, right._value) != 0;
    }

    friend bool operator>(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return mpfr_greater_p(left._value, right._value) != 0;
    }

    friend bool operator>=(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return mpfr_greaterequal_p(left._value, right._value) != 0;
    }

    friend bool operator==(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return mpfr_equal_p(left._value, right._value) != 0;
    }

    friend bool operator!=(const HighPrecisionReal& left, const HighPrecisionReal& right)
    {
        return !(left == right);
    }

    friend HighPrecisionReal abs(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_abs(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal sqrt(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_sqrt(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal sin(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_sin(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal cos(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_cos(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal sinh(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_sinh(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal cosh(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_cosh(result._value, value._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal atan2(const HighPrecisionReal& y, const HighPrecisionReal& x)
    {
        HighPrecisionReal result;
        mpfr_atan2(result._value, y._value, x._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal pow(const HighPrecisionReal& base, const HighPrecisionReal& exponent)
    {
        HighPrecisionReal result;
        mpfr_pow(result._value, base._value, exponent._value, kRoundingMode);
        return result;
    }

    friend HighPrecisionReal log(const HighPrecisionReal& value)
    {
        HighPrecisionReal result;
        mpfr_log(result._value, value._value, kRoundingMode);
        return result;
    }

private:
    static constexpr mpfr_rnd_t kRoundingMode = MPFR_RNDN;
    inline static thread_local mpfr_prec_t _defaultPrecision = 1024;

    mpfr_t _value;
};

template<class Real>
double ToDouble(const Real& value)
{
    if constexpr (std::is_same_v<Real, double>)
        return value;
    else
        return value.ToDouble();
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
PrecisionComplex<Real> ComplexCos(const PrecisionComplex<Real>& z)
{
    using std::cos;
    using std::cosh;
    using std::sin;
    using std::sinh;
    return {cos(z.re) * cosh(z.im), -sin(z.re) * sinh(z.im)};
}

template<class Real>
PrecisionComplex<Real> ComplexTan(const PrecisionComplex<Real>& z)
{
    return ComplexSin(z) / ComplexCos(z);
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
