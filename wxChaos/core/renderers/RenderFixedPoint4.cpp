#include <complex>
#include "RenderFixedPoint4.h"
using namespace std;

RenderFixedPoint4::RenderFixedPoint4()
{
    _minStep = 0.01;
}
void RenderFixedPoint4::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = pow(z,2);

                if((z_prev.real() - _minStep < z.real() && z_prev.real() + _minStep > z.real())
                    && (z_prev.imag() - _minStep < z.imag() && z_prev.imag() + _minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint4::SetParams(double minStep)
{
    _minStep = minStep;
}

