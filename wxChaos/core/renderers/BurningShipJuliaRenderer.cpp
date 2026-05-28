#include "BurningShipJuliaRenderer.h"
#include "FractalUtils.h"

BurningShipJuliaRenderer::BurningShipJuliaRenderer() = default;

void BurningShipJuliaRenderer::EscapeTimeRender()
{
    const double c_im = _kImaginary;
    const double c_re = _kReal;
    unsigned n;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = _maxY - _y * _yFactor;
            double Z_im = _minX + _x * _xFactor;
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

void BurningShipJuliaRenderer::GaussianIntRender()
{
    const double c_im = _kImaginary;
    const double c_re = _kReal;
    double distance1;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = _minX + _x * _xFactor;
            double Z_im = _maxY - _y * _yFactor;
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
                    if (n > 0)
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

void BurningShipJuliaRenderer::EscapeAngleRender()
{
    const double c_im = _kImaginary;
    const double c_re = _kReal;
    unsigned n;
    const int color1 = 1;
    const int color2 = 0.25 * _myOpt.paletteSize;
    const int color3 = 0.50 * _myOpt.paletteSize;
    const int color4 = 0.75 * _myOpt.paletteSize;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = _minX + _x * _xFactor;
            double Z_im = _maxY - _y * _yFactor;
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

void BurningShipJuliaRenderer::Render()
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

