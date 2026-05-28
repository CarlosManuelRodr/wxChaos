#include <complex>
#include "SinoidalRenderer.h"
#include "FractalUtils.h"
using namespace std;

SinoidalRenderer::SinoidalRenderer() {}
void SinoidalRenderer::EscapeTimeRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;
    const complex<double> k(_kReal, _kImaginary);
    bool insideSet;
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
                    if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = k*sin(z);
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }

}

void SinoidalRenderer::GaussianIntRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;
    const complex<double> k(_kReal, _kImaginary);
    bool insideSet;
    double c_im;


        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (n=0; n<_maxIter && insideSet; n++)
                {
                    const double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if (zNorm > _maxIter)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if (n > 0) insideSet = false;
                    }
                    z = k*sin(z);

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
            }
        }

}

void SinoidalRenderer::EscapeAngleRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;
    const complex<double> k(_kReal, _kImaginary);
    bool insideSet;
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
                    z = k*sin(z);
                    if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
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

void SinoidalRenderer::Render()
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
void SinoidalRenderer::EscapeTimeWithOrbitTrapRender()
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

            double distanceX = abs(re);
            double distanceY = abs(im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<_maxIter; n++)
            {
                z = constant*sin(z);
                if (z.real()*z.real() + z.imag()*z.imag() > _maxIter)
                {
                    insideSet = false;
                    broken = true;
                }

                if (_myOpt.orbitTrapMode)
                {
                    if (abs(z.imag()) < distanceY)
                        distanceY = abs(z.imag());
                    if (abs(z.real()) < distanceX)
                        distanceX = abs(z.real());
                }

                if (!broken)
                    iterations = n;
            }

            if (insideSet)
                _setMap[_x][_y] = true;
            if (_myOpt.orbitTrapMode)
                _colorMap[_x][_y] = static_cast<unsigned int>(abs(iterations + log(1/distanceX) + log(1/distanceY)));
            else
                _colorMap[_x][_y] = iterations;
        }
    }
}

