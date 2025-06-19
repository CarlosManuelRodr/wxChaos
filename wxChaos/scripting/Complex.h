#pragma once

#include <complex>

struct Complex
{
    Complex();
    Complex(const Complex& other);
    explicit Complex(const std::complex<double>& other);
    explicit Complex(double r, double i = 0);

    Complex &operator=(const Complex& other);
    Complex &operator+=(const Complex& other);
    Complex &operator-=(const Complex& other);
    Complex &operator*=(const Complex& other);
    Complex &operator/=(const Complex& other);

    double norm() const;
    double squaredNorm() const;

    bool operator==(const Complex& other) const;
    bool operator!=(const Complex& other) const;

    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;
    Complex operator/(const Complex& other) const;

    std::complex<double> complexNum;
    double real();
    double imag();
};

void asPrintComplex(const Complex& num);

Complex cpxPow(const Complex &base, const int &exponent);
Complex cpxPow(const Complex &base, const double &exponent);
Complex cpxPow(const Complex &base, const Complex &exponent);
Complex cpxSqrt(const Complex &z);
Complex cpxSin(const Complex &z);
Complex cpxCsc(const Complex &z);
Complex cpxCos(const Complex &z);
Complex cpxSec(const Complex &z);
Complex cpxTan(const Complex &z);
Complex cpxCot(const Complex &z);
Complex cpxSinh(const Complex &z);
Complex cpxCosh(const Complex &z);
Complex cpxTanh(const Complex &z);
Complex cpxExp(const Complex &z);
Complex cpxLog(const Complex &z);
Complex cpxLog10(const Complex &z);

