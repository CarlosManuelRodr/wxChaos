#include <complex>
#include "FixedPoint2Renderer.h"
using namespace std;

FixedPoint2Renderer::FixedPoint2Renderer()
{
    _minStep = 0.01;
}
void FixedPoint2Renderer::Render()
{
    complex<double> z;
    unsigned n;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            complex<double> z_prev = z = complex<double>(_minX + _x * _xFactor, _maxY - _y * _yFactor);

            for (n=0; n<_maxIter; n++)
            {
                z = cos(z);

                if ((z_prev.real() - _minStep < z.real() && z_prev.real() + _minStep > z.real())
                    && (z_prev.imag() - _minStep < z.imag() && z_prev.imag() + _minStep > z.imag()))
                    break;

                z_prev = z;
            }
            _colorMap[_x][_y] = (z.real() > 0 ? 1 : 30) + n;
        }
    }
}
void FixedPoint2Renderer::SetParams(double minStep)
{
    _minStep = minStep;
}

