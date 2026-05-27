#include <complex>
#include "ManowarJuliaRenderer.h"
#include "FractalUtils.h"
using namespace std;

ManowarJuliaRenderer::ManowarJuliaRenderer() {}
void ManowarJuliaRenderer::Render()
{
    // Creates fractal.
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double z_y_init;

    if (_myOpt.alg == RenderingAlgorithmType::EscapeTime)
    {
        unsigned n;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            z_y_init = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                man_re = z_re = _minX + _x*_xFactor;
                man_im = z_im = z_y_init;
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if (z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    const double temp_re = z_re;
                    const double temp_im = z_im;
                    z_im = 2*z_re*z_im + _kImaginary + man_im;
                    z_re = z_re2 - z_im2 + _kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }
    }
    else if (_myOpt.alg == RenderingAlgorithmType::GaussianInt)
    {
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            z_y_init = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                man_re = z_re = _minX + _x*_xFactor;
                man_im = z_im = z_y_init;

                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (unsigned n = 0; n<_maxIter && insideSet; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;

                    if (z_re2 + z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z_re2 + z_im2))))/log2 + 1;
                        if (n > 0) insideSet = false;
                    }
                    const double temp_re = z_re;
                    const double temp_im = z_im;
                    z_im = 2*z_re*z_im + _kImaginary + man_im;
                    z_re = z_re2 - z_im2 + _kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z_re, z_im));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
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

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            z_y_init = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                man_re = z_re = _minX + _x*_xFactor;
                man_im = z_im = z_y_init;
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if (z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    const double temp_re = z_re;
                    const double temp_im = z_im;
                    z_im = 2.0 * z_re * z_im + _kImaginary + man_im;
                    z_re = z_re2 - z_im2 + _kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                if (z_re > 0 && z_im > 0)
                    _colorMap[_x][_y] = n + color1;
                else if (z_re <= 0 && z_im > 0)
                    _colorMap[_x][_y] = n + color2;
                else if (z_re <= 0 && z_im < 0)
                    _colorMap[_x][_y] = n + color3;
                else
                    _colorMap[_x][_y] = n + color4;
            }
        }
    }
}
void ManowarJuliaRenderer::SpecialRender()
{
    // Creates fractal.
    complex<double> z;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double re = _minX + _x * _xFactor;
            double im = _maxY - _y * _yFactor;
            auto man = z = complex<double>(re, im);
            auto constant = complex<double>(_kReal, _kImaginary);
            bool broken = false;

            double distX = abs(re);
            double distY = abs(im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<_maxIter; n++)
            {
                if (z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                const complex<double> temp = z;
                z = pow(z, 2) + man + constant;
                man = temp;

                if (_myOpt.orbitTrapMode)
                {
                    if (abs(z.imag()) < distY) distY = abs(z.imag());
                    if (abs(z.real()) < distX) distX = abs(z.real());
                }

                if (!broken)
                    iterations = n;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            if (distX == 0)
                distX = 0.000001;
            if (distY == 0)
                distY = 0.000001;

            if (_myOpt.orbitTrapMode)
                _colorMap[_x][_y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                _colorMap[_x][_y] = iterations;
        }
    }
}

