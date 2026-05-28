#include <complex>
#include "MagnetRenderer.h"
using namespace std;

MagnetRenderer::MagnetRenderer()
{

}
void MagnetRenderer::EscapeTimeRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> c;
    unsigned n;
    bool insideSet;
    double c_im;


        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(0, 0);
                c = complex<double>(_minX + _x*_xFactor, c_im);

                insideSet = true;
                for (n=0; n<_maxIter; n++)
                {

                    if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
                }
                if (insideSet)
                    _setMap[_x][_y] = true;
                _colorMap[_x][_y] = n;
            }
        }

}

void MagnetRenderer::EscapeAngleRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> c;
    unsigned n;
    bool insideSet;
    double c_im;


        constexpr int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(0, 0);
                c = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
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

void MagnetRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender();
            break;
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender();
            break;
        default:
            break;
    }
}

