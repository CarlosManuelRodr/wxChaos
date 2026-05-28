#include <complex>
#include "FractoryRenderer.h"
#include "FractalUtils.h"
using namespace std;

FractoryRenderer::FractoryRenderer()
{

}
void FractoryRenderer::EscapeTimeRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                c = complex<double>(_minX + _x*_xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    if (z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }

}

void FractoryRenderer::GaussianIntRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;

        double distance1;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {

                c = complex<double>(_minX + _x*_xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (n=0; n<_maxIter && insideSet; n++)
                {
                    if (z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z.real()*z.real()+ z.imag()*z.imag()))))/log2 + 1;
                        if (n > 0) insideSet = false;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
            }
        }

}

void FractoryRenderer::EscapeAngleRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;

        const int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                c = complex<double>(_minX + _x*_xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    if (z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
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

void FractoryRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender();
            break;
        case RenderingAlgorithmType::GaussianInt:
            GaussianIntRender();
            break;
        case RenderingAlgorithmType::EscapeAngle:
            EscapeAngleRender();
            break;
        default:
            break;
    }
}

