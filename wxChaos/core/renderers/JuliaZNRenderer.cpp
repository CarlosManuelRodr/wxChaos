#include <complex>
#include "JuliaZNRenderer.h"
#include "FractalUtils.h"
using namespace std;

JuliaZNRenderer::JuliaZNRenderer()
{
    _n = 0;
    _bailout = 0;
}
void JuliaZNRenderer::EscapeTimeRender()
{
    double c_im;
    bool insideSet;
    unsigned i;

    // Creates fractal.

        const complex<double> k = complex<double>(_kReal, _kImaginary);
        const double squaredBail = _bailout*_bailout;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = complex<double>(_minX + _x * _xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + k;
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

void JuliaZNRenderer::GaussianIntRender()
{
    double c_im;
    bool insideSet;
    unsigned i;

    // Creates fractal.

        const complex<double> k = complex<double>(_kReal, _kImaginary);
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);
        const double squaredBail = _bailout*_bailout;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = complex<double>(_minX + _x * _xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (i=0; i<_maxIter && insideSet; i++)
                {
                    const double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if (zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if (i > 0) insideSet = false;
                    }
                    z = pow(z,_n) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
            }
        }

}

void JuliaZNRenderer::EscapeAngleRender()
{
    double c_im;
    bool insideSet;
    unsigned i;

    // Creates fractal.

        constexpr int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;
        const complex<double> k = complex<double>(_kReal, _kImaginary);
        const double squaredBail = _bailout*_bailout;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                complex<double> z = complex<double>(_minX + _x * _xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + k;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                if (z.real() > 0 && z.imag() > 0)
                    _colorMap[_x][_y] = _n + color1;
                else if (z.real() <= 0 && z.imag() > 0)
                    _colorMap[_x][_y] = _n + color2;
                else if (z.real() <= 0 && z.imag() < 0)
                    _colorMap[_x][_y] = _n + color3;
                else
                    _colorMap[_x][_y] = _n + color4;
            }
        }

}

void JuliaZNRenderer::Render()
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
void JuliaZNRenderer::EscapeTimeSmoothRender()
{
    EscapeTimeWithOrbitTrapRender();
}

void JuliaZNRenderer::EscapeTimeWithOrbitTrapRender()
{
    double c_im;
    bool insideSet;
    complex<double> z;
    const auto k = complex<double>(_kReal, _kImaginary);
    const double log2 = log(2.0);
    unsigned i;
    int out;

    if (_myOpt.orbitTrapMode)
    {
        const double squaredBail = _bailout*_bailout;
        double Z_im2 = 0;
        double Z_re2 = 0;
        const double bailFourPower = squaredBail*squaredBail;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, c_im);

                double distX = abs(z.real());
                double distY = abs(z.imag());

                insideSet = true;
                int iterations = 0;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z, _n) + k;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if (Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if (Z_re2 + Z_im2 > bailFourPower)
                        {
                            if (abs(z.imag()) < distY) distY = abs(z.imag());
                            if (abs(z.real()) < distX) distX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if (abs(z.imag()) < distY)
                        distY = abs(z.imag());
                    if (abs(z.real()) < distX)
                        distX = abs(z.real());
                }
                if (distX == 0)
                    distX = 0.000001;
                if (distY == 0)
                    distY = 0.000001;

                if (insideSet)
                    _setMap[_x][_y] = true;
                if (_myOpt.smoothRender)
                {
                    out = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    if (out < 0)
                        out = 0;
                    if (!insideSet)
                        _colorMap[_x][_y] = out;
                    else
                        _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    _colorMap[_x][_y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    else if (_myOpt.smoothRender)
    {
        const double squaredBail = _bailout*_bailout;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                z = complex<double>(_minX + _x*_xFactor, c_im);
                insideSet = true;

                for (i=0; i<_maxIter; i++)
                {
                    z = pow(z,_n) + k;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                out = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
                if (out < 0) out = 0;
                _colorMap[_x][_y] = out;
            }
        }
    }
}
void JuliaZNRenderer::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

