#include "ManowarRenderer.h"
#include "FractalUtils.h"

ManowarRenderer::ManowarRenderer() = default;

void ManowarRenderer::EscapeTimeRender()
{
    double z_re, z_im;
    double c_re, c_im;
    unsigned n = 0;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double z_y_init = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double man_re = z_re = c_re = _minX + _x * _xFactor;
            double man_im = z_im = c_im = z_y_init;

            bool insideSet = true;
            for (n=0; n<_maxIter; n++)
            {
                double z_re2 = z_re * z_re;
                double z_im2 = z_im * z_im;
                if (z_re2 + z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
                double temp_re = z_re;
                double temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;
            }
            if (insideSet)
            {
                _setMap[_x][_y] = true;
            }
            _colorMap[_x][_y] = n;
        }
    }
}

void ManowarRenderer::GaussianIntRender()
{
    double z_re, z_im;
    double c_re, c_im;
    double distance1 = 0.0;
    const double log2 = log(2.0);
    const double loglog2 = log(log2);

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double z_y_init = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double man_re = z_re = c_re = _minX + _x * _xFactor;
            double man_im = z_im = c_im = z_y_init;
            bool insideSet = true;
            double distance = 99.0;
            double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

            // ReSharper disable once CppDeclarationHidesLocal
            for (unsigned n = 0; n<_maxIter && insideSet; n++)
            {
                double z_re2 = z_re * z_re;
                double z_im2 = z_im * z_im;

                if (z_re2 + z_im2 > 4)
                {
                    mu = (loglog2 - log(log(sqrt(z_re2 + z_im2))))/log2 + 1;
                    insideSet = false;
                }
                double temp_re = z_re;
                double temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;

                distance1 = distance;
                distance = minVal(distance, gaussianIntDist(z_re, z_im));
            }
            if (insideSet)
            {
                _setMap[_x][_y] = true;
            }
            _colorMap[_x][_y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*_myOpt.paletteSize)));
        }
    }
}

void ManowarRenderer::EscapeAngleRender()
{
    double z_re, z_im;
    double c_re, c_im;
    // ReSharper disable once CppTooWideScope
    constexpr int color1 = 1;
    const int color2 = static_cast<int>(0.25 * _myOpt.paletteSize);
    const int color3 = static_cast<int>(0.50 * _myOpt.paletteSize);
    const int color4 = static_cast<int>(0.75 * _myOpt.paletteSize);
    unsigned n = 0;

    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        double z_y_init = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double man_re = z_re = c_re = _minX + _x * _xFactor;
            double man_im = z_im = c_im = z_y_init;

            bool insideSet = true;
            for (n=0; n<_maxIter; n++)
            {
                double z_re2 = z_re * z_re;
                double z_im2 = z_im * z_im;
                if (z_re2 + z_im2 > 4)
                {
                    insideSet = false;
                    break;
                }
                double temp_re = z_re;
                double temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;
            }
            if (insideSet)
                _setMap[_x][_y] = true;

            if (z_re > 0 && z_im > 0)
                _colorMap[_x][_y] = n + color1;
            else if (z_re <= 0 && z_im > 0)
                _colorMap[_x][_y] = n + color2;
            else if (z_re <= 0 && z_im < 0)
                _colorMap[_x][_y] = n + color3;
            else
                _colorMap[_x][_y] = n + color4;
        }
    }

}

void ManowarRenderer::EscapeTimeWithOrbitTrapRender()
{
    double z_re, z_im;
    double c_re;
    for (_y=_heightOrigin; _y<_heightFinal; _y++)
    {
        const double c_im = _maxY - _y * _yFactor;
        for (_x=_widthOrigin; _x<_widthFinal; _x++)
        {
            double man_re = z_re = c_re = _minX + _x * _xFactor;
            double man_im = z_im = c_im;
            bool broken = false;

            double distX = abs(c_re);
            double distY = abs(c_im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<_maxIter; n++)
            {
                const double z_re2 = z_re * z_re;
                const double z_im2 = z_im * z_im;
                if (z_re2 + z_im2 > 4)
                {
                    insideSet = false;
                    broken = true;
                }
                const double temp_re = z_re;
                const double temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;

                if (abs(z_im) < distY)
                    distY = abs(z_im);
                if (abs(z_re) < distX)
                    distX = abs(z_re);

                if (!broken)
                    iterations = n;
            }
            if (distX == 0)
                distX = 0.000001;
            if (distY == 0)
                distY = 0.000001;

            if (insideSet)
                _setMap[_x][_y] = 1;

            _colorMap[_x][_y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
        }
    }
}

void ManowarRenderer::Render()
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