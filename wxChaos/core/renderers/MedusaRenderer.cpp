#include <complex>
#include "MedusaRenderer.h"
#include "FractalUtils.h"
using namespace std;

MedusaRenderer::MedusaRenderer()
{

}
void MedusaRenderer::EscapeTimeRender()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    complex<double> z;
    const auto k = complex<double>(_kReal, _kImaginary);
    double c_im;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;
                for (n=0; n<_maxIter; n++)
                {
                    if (z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if (insideSet)
                {
                    _setMap[_x][_y] = true;
                }
                _colorMap[_x][_y] = n;
            }
        }

}

void MedusaRenderer::GaussianIntRender()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    complex<double> z;
    const auto k = complex<double>(_kReal, _kImaginary);
    double c_im;

        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            double cIm = _maxY - _y * _yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, cIm);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (n=0; n<_maxIter && insideSet; n++)
                {
                    const double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if (zNorm > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if (n > 0) insideSet = false;
                    }
                    z = pow(z, 1.5) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                {
                    _setMap[_x][_y] = true;
                }
                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
            }
        }

}

void MedusaRenderer::EscapeAngleRender()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    complex<double> z;
    const auto k = complex<double>(_kReal, _kImaginary);
    double c_im;

        const int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    if (z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if (insideSet)
                {
                    _setMap[_x][_y] = true;
                }
                if (z.real() > 0 && z.imag() > 0)
                {
                    _colorMap[_x][_y] = n + color1;
                }
                else if (z.real() <= 0 && z.imag() > 0)
                {
                    _colorMap[_x][_y] = n + color2;
                }
                else if (z.real() <= 0 && z.imag() < 0)
                {
                    _colorMap[_x][_y] = n + color3;
                }
                else
                {
                    _colorMap[_x][_y] = n + color4;
                }
            }
        }

}

void MedusaRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            if (_myOpt.orbitTrapMode)
                EscapeTimeWithOrbitTrapRender();
            else
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
void MedusaRenderer::EscapeTimeWithOrbitTrapRender()
{
    // Creates fractal.
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

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<_maxIter; n++)
            {
                z = pow(z, 1.5) + constant;
                if (z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                if (_myOpt.orbitTrapMode)
                {
                    if (abs(z.imag()) < distY)
                        distY = abs(z.imag());
                    if (abs(z.real()) < distX)
                        distX = abs(z.real());
                }

                if (!broken)
                    iterations = n;
            }
            if (insideSet)
            {
                _setMap[_x][_y] = true;
            }
            if (_myOpt.orbitTrapMode)
            {
                _colorMap[_x][_y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            }
            else
            {
                _colorMap[_x][_y] = iterations;
            }
        }
    }
}

