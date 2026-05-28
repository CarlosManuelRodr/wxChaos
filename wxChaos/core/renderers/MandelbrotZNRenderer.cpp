#include <complex>
#include "MandelbrotZNRenderer.h"
#include "FractalUtils.h"
using namespace std;

MandelbrotZNRenderer::MandelbrotZNRenderer()
{
    _n = 0;
    _bailout = 0;
}
void MandelbrotZNRenderer::EscapeTimeRender()
{
    double c_im;
    bool insideSet;

    // Creates fractal.
    unsigned i;
    complex<double> z, c;
    const double squaredBail = _bailout*_bailout;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = c = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = i;
            }
        }

}

void MandelbrotZNRenderer::GaussianIntRender()
{
    double c_im;
    bool insideSet;

    // Creates fractal.
    unsigned i;
    complex<double> z, c;
    const double squaredBail = _bailout*_bailout;

        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(0,0);
                c = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (i=0; i<_maxIter && insideSet; i++)
                {
                    const double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if (zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        insideSet = false;
                    }
                    z = pow(z,_n) + c;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
            }
        }

}

void MandelbrotZNRenderer::EscapeAngleRender()
{
    double c_im;
    bool insideSet;

    // Creates fractal.
    unsigned i;
    complex<double> z, c;
    const double squaredBail = _bailout*_bailout;

        // ReSharper disable once CppTooWideScope
        constexpr int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = c = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                if (z.real() > 0 && z.imag() > 0)
                    _colorMap[_x][_y] = i + color1;
                else if (z.real() <= 0 && z.imag() > 0)
                    _colorMap[_x][_y] = i + color2;
                else if (z.real() <= 0 && z.imag() < 0)
                    _colorMap[_x][_y] = i + color3;
                else
                    _colorMap[_x][_y] = i + color4;
            }
        }

}

void MandelbrotZNRenderer::Render()
{
    switch (_myOpt.alg)
    {
        case RenderingAlgorithmType::EscapeTime:
            if (_myOpt.orbitTrapMode)
                EscapeTimeWithOrbitTrapRender();
            else if (_myOpt.smoothRender)
                EscapeTimeSmoothRender();
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
void MandelbrotZNRenderer::EscapeTimeSmoothRender()
{
    EscapeTimeWithOrbitTrapRender();
}

void MandelbrotZNRenderer::EscapeTimeWithOrbitTrapRender()
{
    double c_im;
    bool insideSet;
    const double squaredBail = _bailout*_bailout;
    const double log2 = log(2.0);
    unsigned i;

    if (_myOpt.orbitTrapMode)
    {
        double Z_im2 = 0;
        double Z_re2 = 0;

        // Creates fractal.
        complex<double> c;
        const double bailFourPower = squaredBail * squaredBail;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = c = complex<double>(_minX + _x * _xFactor, c_im);

                double distanceX = abs(z.real());
                double distanceY = abs(z.imag());

                insideSet = true;
                int iterations = 0;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z, _n) + c;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if (Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if (Z_re2 + Z_im2 > bailFourPower)
                        {
                            if (abs(z.imag()) < distanceY)
                                distanceY = abs(z.imag());
                            if (abs(z.real()) < distanceX)
                                distanceX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if (abs(z.imag()) < distanceY)
                        distanceY = abs(z.imag());
                    if (abs(z.real()) < distanceX)
                        distanceX = abs(z.real());
                }
                if (distanceX == 0)
                    distanceX = 0.000001;
                if (distanceY == 0)
                    distanceY = 0.000001;

                if (insideSet)
                    _setMap[_x][_y] = true;

                if (_myOpt.smoothRender)
                {
                    if (!insideSet)
                        _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distanceX) + log(1/distanceY))));
                    else
                        _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distanceX) + log(1/distanceY)))));
                }
                else
                    _colorMap[_x][_y] = static_cast<int>(abs(iterations + log(1/distanceX) + log(1/distanceY)));
            }
        }
    }
    else if (_myOpt.smoothRender)
    {
        // Creates fractal.
        complex<double> c;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = c = complex<double>(_minX + _x * _xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
            }
        }
    }
}
void MandelbrotZNRenderer::SetParams(int n, double bailout)
{
    _n = n;
    _bailout = bailout;
}

