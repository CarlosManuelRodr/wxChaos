#include <complex>
#include "CellRenderer.h"
#include "FractalUtils.h"
using namespace std;

CellRenderer::CellRenderer()
{
    _bailout = 1.0;
}
void CellRenderer::EscapeTimeRender()
{
    // Creates fractal.
    const double squaredBail = _bailout*_bailout;
    unsigned n;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            complex<double> c = complex<double>(_minX + _x * _xFactor, c_im);
            complex<double> z = c;
            complex<double> b = c - sin(c);
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                if (z.real()*z.real()+ z.imag()*z.imag() > squaredBail)
                {
                    insideSet = false;
                    break;
                }
                b /= c;
                z = z*c + b/z;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            _colorMap[_x][_y] = n;
        }
    }
}

void CellRenderer::GaussianIntRender()
{
    // Creates fractal.
    const double squaredBail = _bailout*_bailout;

    double distance1 = 0;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            complex<double> c = complex<double>(_minX + _x * _xFactor, c_im);
            complex<double> z = c;
            complex<double> b = c - sin(c);
            bool insideSet = true;
            double distance = 99;
            double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

            for (unsigned n = 0; n<_maxIter && insideSet; n++)
            {
                const double norm = z.real() * z.real() + z.imag() * z.imag();
                if (norm > squaredBail)
                {
                    mu = (loglog2 - log(log(sqrt(norm))))/log2 + 1;
                    if (n > 0) insideSet = false;
                }
                b /= c;
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

void CellRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            EscapeTimeRender();
            break;
        case RenderingAlgorithmType::GaussianInt:
            GaussianIntRender();
            break;
        default:
            break;
    }
}
void CellRenderer::SetParams(double bailout)
{
    _bailout = bailout;
}

