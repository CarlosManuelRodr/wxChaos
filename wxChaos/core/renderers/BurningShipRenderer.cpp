// ReSharper disable CppTooWideScope
#include "BurningShipRenderer.h"
#include "FractalUtils.h"

BurningShipRenderer::BurningShipRenderer() = default;

void BurningShipRenderer::EscapeTimeRender()
{
    unsigned n;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = 0;
            double Z_im = 0;
            double c_re = _minX + _x * _xFactor;
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;
                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
                Z_im = abs(Z_im);
                Z_re = abs(Z_re);
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            _colorMap[_x][_y] = n;
        }
    }

}

void BurningShipRenderer::GaussianIntRender()
{
    double distance1 = 0;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_im;
            double Z_re = Z_im = 0;
            double c_re = _minX + _x * _xFactor;
            bool insideSet = true;
            double distance = 99;
            double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

            for (unsigned n = 0; n<_maxIter && insideSet; n++)
            {
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4)
                {
                    mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                    insideSet = false;
                }
                Z_im = abs(Z_im);
                Z_re = abs(Z_re);
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;

                distance1 = distance;
                distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
        }
    }

}

void BurningShipRenderer::EscapeAngleRender()
{
    unsigned n;
    constexpr int color1 = 1;
    const int color2 = static_cast<int>(0.25 * _myOpt.paletteSize);
    const int color3 = static_cast<int>(0.50 * _myOpt.paletteSize);
    const int color4 = static_cast<int>(0.75 * _myOpt.paletteSize);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = 0;
            double Z_im = 0;
            double c_re = _minX + _x * _xFactor;
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                double Z_re2 = Z_re * Z_re;
                double Z_im2 = Z_im * Z_im;
                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
                Z_im = abs(Z_im);
                Z_re = abs(Z_re);
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            if (Z_re > 0 && Z_im > 0)
                _colorMap[_x][_y] = n + color1;
            else if (Z_re <= 0 && Z_im > 0)
                _colorMap[_x][_y] = n + color2;
            else if (Z_re <= 0 && Z_im < 0)
                _colorMap[_x][_y] = n + color3;
            else
                _colorMap[_x][_y] = n + color4;
        }
    }

}

void BurningShipRenderer::Render()
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

