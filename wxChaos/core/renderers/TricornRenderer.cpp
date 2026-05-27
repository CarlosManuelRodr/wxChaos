#include "TricornRenderer.h"
#include "FractalUtils.h"

TricornRenderer::TricornRenderer()
{

}
void TricornRenderer::Render()
{
    double Z_re, Z_im, Z_re2, Z_im2;
    double c_re, c_im;
    bool insideSet;

    // Creates fractal.
    if (_myOpt.alg == RenderingAlgorithmType::EscapeTime)
    {
        unsigned n;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                Z_re = c_re = _minX + _x*_xFactor;
                Z_im = c_im;
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if (Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = -Z_im;
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }
    }
    else if (_myOpt.alg == RenderingAlgorithmType::GaussianInt)
    {
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                Z_re = Z_im = 0;
                c_re = _minX + _x*_xFactor;
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (unsigned n = 0; n<_maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if (Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        insideSet = false;
                    }
                    Z_im = -Z_im;
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
    else if (_myOpt.alg == RenderingAlgorithmType::EscapeAngle)
    {
        unsigned n;
        constexpr int color1 = 1;
        const int color2 = 0.25 * _myOpt.paletteSize;
        const int color3 = 0.50 * _myOpt.paletteSize;
        const int color4 = 0.75 * _myOpt.paletteSize;

        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            c_im = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                Z_re = c_re = _minX + _x*_xFactor;
                Z_im = c_im;
                insideSet = true;

                for (n=0; n<_maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if (Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = -Z_im;
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
}

