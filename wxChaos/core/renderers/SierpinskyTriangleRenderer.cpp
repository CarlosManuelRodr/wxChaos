#include <mpParser.h>
#include "SierpinskyTriangleRenderer.h"
using namespace std;

SierpinskyTriangleRenderer::SierpinskyTriangleRenderer() = default;

void SierpinskyTriangleRenderer::Render()
{
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            auto z = complex<double>(_minX + _x * _xFactor, _maxY - _y * _yFactor);
            bool insideSet = true;
            int iterations = 0;
            for (unsigned n=0; n<_maxIter; n++)
            {
                if (z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    break;
                }

                if (z.imag() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(0, 1);
                else if (z.real() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(1, 0);
                else
                    z = complex<double>(2, 0)*z;

                iterations = n;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            _colorMap[_x][_y] = iterations;
        }
    }
}

