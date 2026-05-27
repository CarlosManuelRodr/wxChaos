#include <complex>
#include "NewtonRenderer.h"
using namespace std;

NewtonRenderer::NewtonRenderer()
{
    minStep = 0.001;
}
void NewtonRenderer::Render()
{
    double re, im;

    // Creates fractal.
    unsigned n;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double prev_re = re = _minX + _x * _xFactor;
            double prev_im = im = _maxY - _y * _yFactor;

            for (n=0; n<_maxIter; n++)
            {
                const double re2 = re * re;
                const double im2 = im * im;
                double d = 3.0 * ((re2 - im2) * (re2 - im2) + 4.0 * re2 * im2);
                if (d == 0.0)
                    d = 0.000001;

                const double tmp = re;
                re = (2.0/3.0)*re + (re2 - im2)/d;
                im = (2.0/3.0)*im - 2.0*tmp*im/d;

                if ((prev_re - minStep < re && prev_re + minStep > re) && (prev_im - minStep < im && prev_im + minStep > im))
                    break;

                prev_re = re;
                prev_im = im;
            }
            if (re <= 0 && im >= 0)
                _colorMap[_x][_y] = 1 + n;
            else if (re <= 0 && im < 0)
                _colorMap[_x][_y] = 17 + n;
            else
                _colorMap[_x][_y] = 37 + n;
        }
    }
}
void NewtonRenderer::SpecialRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double re = _minX + _x * _xFactor;
            double im = _maxY - _y * _yFactor;
            complex<double> z_prev = z = complex<double>(re, im);

            double distX = abs(re);
            double distY = abs(im);

            for (n=0; n<_maxIter; n++)
            {
                z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

                if ((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;

                z_prev = z;

                if (_myOpt.orbitTrapMode)
                {
                    if (abs(z.imag()) < distY) distY = abs(z.imag());
                    if (abs(z.real()) < distX) distX = abs(z.real());
                }
            }
            if (_myOpt.orbitTrapMode)
                _colorMap[_x][_y] = static_cast<unsigned int>(n + log(1/distX) + log(1/distY));
            else
                _colorMap[_x][_y] = n;
        }
    }
}
void NewtonRenderer::SetParams(double _minStep)
{
    minStep = _minStep;
}

