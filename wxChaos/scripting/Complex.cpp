// ReSharper disable CppMemberFunctionMayBeConst
#include "Complex.h"

Complex::Complex()
{
    complexNum = std::complex<double>(0,0);
}
Complex::Complex(const Complex &other)
{
    complexNum = other.complexNum;
}
Complex::Complex(const std::complex<double>& other)
{
    complexNum = other;
}
Complex::Complex(const double r, const double i)
{
    complexNum = std::complex<double>(r, i);
}
bool Complex::operator==(const Complex &o) const
{
    return (complexNum == o.complexNum);
}
bool Complex::operator!=(const Complex &o) const
{
    return !(*this == o);
}
Complex &Complex::operator=(const Complex &other) = default;
Complex &Complex::operator+=(const Complex &other)
{
    complexNum += other.complexNum;
    return *this;
}
Complex &Complex::operator-=(const Complex &other)
{
    complexNum -= other.complexNum;
    return *this;
}
Complex &Complex::operator*=(const Complex &other)
{
    *this = *this * other;
    return *this;
}
Complex &Complex::operator/=(const Complex &other)
{
    *this = *this / other;
    return *this;
}
double Complex::norm() const
{
    return std::norm(complexNum);
}
double Complex::squaredNorm() const
{
    return complexNum.real()*complexNum.real() + complexNum.imag()*complexNum.imag();
}
Complex Complex::operator+(const Complex &other) const
{
    return Complex(complexNum + other.complexNum);
}
Complex Complex::operator-(const Complex &other) const
{
    return Complex(complexNum - other.complexNum);
}
Complex Complex::operator*(const Complex &other) const
{
    return Complex(complexNum*other.complexNum);
}
Complex Complex::operator/(const Complex &other) const
{
    return Complex(complexNum/other.complexNum);
}
double Complex::real()
{
    return complexNum.real();
}
double Complex::imag()
{
    return complexNum.imag();
}

Complex cpxPow(const Complex &base, const int &exponent)
{
    return Complex(std::pow(base.complexNum, exponent));
}
Complex cpxPow(const Complex &base, const double &exponent)
{
    return Complex(std::pow(base.complexNum, exponent));
}
Complex cpxPow(const Complex &base, const Complex &exponent)
{
    return Complex(std::pow(base.complexNum, exponent.complexNum));
}
Complex cpxSqrt(const Complex &z)
{
    return Complex(sqrt(z.complexNum));
}
Complex cpxSin(const Complex &z)
{
    return Complex(sin(z.complexNum));
}
Complex cpxCsc(const Complex &z)
{
    return Complex(std::complex<double>(1,0)/sin(z.complexNum));
}
Complex cpxCos(const Complex &z)
{
    return Complex(cos(z.complexNum));
}
Complex cpxSec(const Complex &z)
{
    return Complex(std::complex<double>(1,0)/cos(z.complexNum));
}
Complex cpxTan(const Complex &z)
{
    return Complex(tan(z.complexNum));
}
Complex cpxCot(const Complex &z)
{
    return Complex(std::complex<double>(1,0)/tan(z.complexNum));
}
Complex cpxSinh(const Complex &z)
{
    return Complex(sinh(z.complexNum));
}
Complex cpxCosh(const Complex &z)
{
    return Complex(cosh(z.complexNum));
}
Complex cpxTanh(const Complex &z)
{
    return Complex(tanh(z.complexNum));
}
Complex cpxExp(const Complex &z)
{
    return Complex(exp(z.complexNum));
}
Complex cpxLog(const Complex &z)
{
    return Complex(log(z.complexNum));
}
Complex cpxLog10(const Complex &z)
{
    return Complex(log10(z.complexNum));
}
