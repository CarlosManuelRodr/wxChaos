#include <complex>
#include "JuliaRenderer.h"
#include "FractalUtils.h"
using namespace std;

JuliaRenderer::JuliaRenderer() {}
void JuliaRenderer::Render()
{
    double c_im;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    // Creates fractal.
    if (_myOpt.alg == RenderingAlgorithmType::EscapeTime)
    {
        unsigned n;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                Z_re = _minX + _x*_xFactor;
                Z_im = c_im;

                insideSet = true;
                for (n=0; n<_maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if (Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + _kImaginary;
                    Z_re = Z_re2 - Z_im2 + _kReal;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }
    }
    else if (_myOpt.alg == RenderingAlgorithmType::GaussianInt)
    {
        double distanceTemp = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            const double temp_im = _maxY - _y * _yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                Z_re = _minX + _x*_xFactor;
                Z_im = temp_im;

                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (unsigned n = 0; n<_maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if (Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if (n > 0)
                            insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im + _kImaginary;
                    Z_re = Z_re2 - Z_im2 + _kReal;

                    distanceTemp = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<int>(abs(((mu*distance + (1-mu)*distanceTemp)*_myOpt.paletteSize)));
            }
        }
    }
    else if (_myOpt.alg == RenderingAlgorithmType::EscapeAngle)
    {
        unsigned n;
        constexpr int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;
        const auto k = complex<double>(_kReal, _kImaginary);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = complex<double>(_minX + _x * _xFactor, c_im);
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    z = pow(z,2) + k;
                    if (z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                if (z.real() > 0 && z.imag() > 0)
                    _colorMap[_x][_y] = n + color1;
                else if (z.real() <= 0 && z.imag() > 0)
                    _colorMap[_x][_y] = n + color2;
                else if (z.real() <= 0 && z.imag() < 0)
                    _colorMap[_x][_y] = n + color3;
                else
                    _colorMap[_x][_y] = n + color4;
            }
        }
    }
    else if (_myOpt.alg == RenderingAlgorithmType::TriangleInequality)
    {
        unsigned n;
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                const double c_re = Z_re = _minX + _x * _xFactor;
                Z_im = c_im;
                insideSet = true;
                double distance = 0;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (n=0; n<_maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if (Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if (n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im;
                    Z_re = Z_re2 - Z_im2;

                    const double tia_prev_x = Z_re;
                    const double tia_prev_y = Z_im;

                    Z_re += _kReal;
                    Z_im += _kImaginary;

                    distance1 = distance;
                    if (n > 0)
                        distance += TIA(Z_re, Z_im, c_re, c_im, tia_prev_x, tia_prev_y);
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                distance1 = distance1/(n-1);
                distance = distance/n;
                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*700)));
            }
        }
    }
}
void JuliaRenderer::SpecialRender()
{
    bool insideSet;
    const double log2 = log(2.0);

    if (_myOpt.orbitTrapMode)
    {
        // Creates fractal.
        complex<double> temp;
        const complex<double> constant(_kReal, _kImaginary);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                double re = _minX + _x * _xFactor;
                double im = _maxY - _y * _yFactor;
                complex<double> z = complex<double>(re, im);

                bool broken = false;
                double distX = abs(re);
                double distY = abs(im);
                insideSet = true;
                int iterations = 0;

                for (unsigned n=0; n<_maxIter; n++)
                {
                    z = pow(z, 2) + constant;
                    if (z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        if (!broken) temp = z;
                        insideSet = false;
                        broken = true;
                    }
                    if (abs(z.imag()) < distY)
                        distY = abs(z.imag());
                    if (abs(z.real()) < distX)
                        distX = abs(z.real());

                    if (!broken)
                        iterations = n;
                }
                if (insideSet)
                {
                    _setMap[_x][_y] = true;
                }
                if (distX == 0)
                    distX = 0.000001;
                if (distY == 0)
                    distY = 0.000001;
                if (_myOpt.smoothRender)
                {
                    if (!insideSet)
                    {
                        int out = static_cast<int>(abs(
                            4.0 * (iterations - log(log(pow(temp.real(), 2) + pow(temp.imag(), 2))) / log2) + 4.0 * (log(1 / distX) +
                                log(1 / distY))));
                        if (out < 0)
                            out = 0;
                        _colorMap[_x][_y] = out;
                    }
                    else
                        _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    _colorMap[_x][_y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    if (_myOpt.smoothRender && !(_myOpt.orbitTrapMode))
    {
        double Z_im2 = 0;
        double Z_re2 = 0;
        unsigned n;
        const double c_re = _kReal;
        const double c_im = _kImaginary;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                double Z_re = _minX + _x * _xFactor;
                double Z_im = _maxY - _y * _yFactor;
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if (Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;
                int out = static_cast<int>(abs(4.0 * (n - log(log(Z_re2 + Z_im2)) / log2)));
                if (out < 0)
                    out = 0;
                _colorMap[_x][_y] = out;
            }
        }
    }
}

