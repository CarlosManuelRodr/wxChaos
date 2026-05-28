#include "MandelbrotRenderer.h"
#include "FractalUtils.h"

MandelbrotRenderer::MandelbrotRenderer()
{
    _buddhaRandomP = 0;
    _bd = 0;
}
void MandelbrotRenderer::EscapeTimeRender()
{
    unsigned n;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            const double c_re = _minX + _x * _xFactor;
            double Z_re = c_re;
            double Z_im = c_im;
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                const double Z_re2 = Z_re * Z_re;
                const double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }

                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }

            if (insideSet)
                _setMap[_x][_y] = true;

            _colorMap[_x][_y] = n;
        }
    }
}

void MandelbrotRenderer::EscapeTimeSmoothRender()
{
    unsigned n;
    const double log2 = log(2.0);
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double c_re;
            double Z_re = c_re = _minX + _x * _xFactor;
            double Z_im = c_im;
            double Z_re2 = 0;
            double Z_im2 = 0;
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                Z_re2 = Z_re*Z_re;
                Z_im2 = Z_im*Z_im;
                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;
            }

            if (insideSet)
                _setMap[_x][_y] = true;
            _colorMap[_x][_y] = static_cast<unsigned int>(abs(4.0*(n -  log(log(Z_re2+Z_im2))/log2)));
        }
    }
}

void MandelbrotRenderer::EscapeTimeWithOrbitTrapRender()
{
    double c_re;
    double Z_re2 = 0;
    double Z_im2 = 0;
    const double log2 = log(2.0);
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_re = c_re = _minX + _x * _xFactor;
            double Z_im = c_im;

            double distX = abs(Z_re);
            double distY = abs(Z_im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<_maxIter; n++)
            {
                Z_re2 = Z_re*Z_re;
                Z_im2 = Z_im*Z_im;
                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    if (Z_re2 + Z_im2 > 16)
                    {
                        if (abs(Z_im) < distY)
                            distY = abs(Z_im);
                        if (abs(Z_re) < distX)
                            distX = abs(Z_re);
                        break;
                    }
                }
                else iterations = n;

                Z_im = 2*Z_re*Z_im + c_im;
                Z_re = Z_re2 - Z_im2 + c_re;

                if (abs(Z_im) < distY)
                    distY = abs(Z_im);
                if (abs(Z_re) < distX)
                    distX = abs(Z_re);
            }
            if (distX == 0)
                distX = 0.000001;
            if (distY == 0)
                distY = 0.000001;

            if (insideSet)
                _setMap[_x][_y] = true;

            if (_myOpt.smoothRender)
            {
                if (!insideSet)
                    _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                else
                    _colorMap[_x][_y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
            }
            else
                _colorMap[_x][_y] = static_cast<int>(iterations + log(1/distX) + log(1/distY));
        }
    }
}

void MandelbrotRenderer::GaussianIntRender()
{
    double distance1 = 0;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_im;
            double Z_re = Z_im = 0;
            const double c_re = _minX + _x * _xFactor;
            bool insideSet = true;
            double distance = 99;
            double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

            for (unsigned n = 0; n<_maxIter && insideSet; n++)
            {
                const double Z_re2 = Z_re * Z_re;
                const double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4)
                {
                    mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                    insideSet = false;
                }

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

void MandelbrotRenderer::EscapeAngleRender()
{
    unsigned n;
    const int color1 = 1;
    const int color2 = 0.25 * _myOpt.paletteSize;
    const int color3 = 0.50 * _myOpt.paletteSize;
    const int color4 = 0.75 * _myOpt.paletteSize;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double c_re;
            double Z_re = c_re = _minX + _x * _xFactor;
            double Z_im = c_im;
            bool insideSet = true;

            for (n=0; n<_maxIter; n++)
            {
                const double Z_re2 = Z_re * Z_re;
                const double Z_im2 = Z_im * Z_im;
                if (Z_re2 + Z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
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

void MandelbrotRenderer::TriangleInequalityRender()
{
    unsigned n;
    double distance1 = 0;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double Z_im;
            double Z_re = Z_im = 0;
            const double c_re = _minX + _x * _xFactor;
            bool insideSet = true;
            double distance = 0;
            double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

            for (n=0; n<_maxIter && insideSet; n++)
            {
                const double Z_re2 = Z_re * Z_re;
                const double Z_im2 = Z_im * Z_im;

                if (Z_re2 + Z_im2 > 4)
                {
                    mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                    if (n > 0) insideSet = false;
                }

                Z_im = 2*Z_re*Z_im;
                Z_re = Z_re2 - Z_im2;

                const double tia_prev_x = Z_re;
                const double tia_prev_y = Z_im;

                Z_re += c_re;
                Z_im += c_im;

                distance1 = distance;
                if (n > 0) distance += TIA(Z_re, Z_im, c_re, c_im, tia_prev_x, tia_prev_y);
            }

            if (insideSet)
                _setMap[_x][_y] = true;

            distance1 = distance1/(n-1);
            distance = distance/n;
            _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*700)));
        }
    }
}
void MandelbrotRenderer::Render()
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
        case RenderingAlgorithmType::TriangleInequality:
            TriangleInequalityRender();
            break;
        default:
            break;
    }
}
void MandelbrotRenderer::SetBuddhaRandomP(int n)
{
    _buddhaRandomP = n;
}
unsigned int MandelbrotRenderer::GetProgress()
{
    if (!_stopped)
    {
        _threadProgress = static_cast<int>(100.0 * (static_cast<double>(_y + 1 - _oldHeightOrigin) / static_cast<double>(_heightFinal - _oldHeightOrigin)));
    }
    return _threadProgress;
}

