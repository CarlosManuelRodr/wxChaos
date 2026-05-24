#include "RenderManowar.h"
#include "FractalUtils.h"

RenderManowar::RenderManowar() {}
void RenderManowar::Render()
{
    // Creates fractal.
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double c_re, c_im, temp_re, temp_im;
    double z_y_init;

    if (myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        unsigned n = 0;
        for (y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for (n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if (z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if (insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = n;
            }
        }
    }
    else if (myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance1 = 0.0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;
                insideSet = true;
                double distance = 99.0;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                // ReSharper disable once CppDeclarationHidesLocal
                for (unsigned n = 0; n<maxIter && insideSet; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;

                    if(z_re2 + z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z_re2 + z_im2))))/log2 + 1;
                        insideSet = false;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z_re, z_im));
                }
                if (insideSet)
                {
                    setMap[x][y] = true;
                }
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if (myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        // ReSharper disable once CppTooWideScope
        constexpr int color1 = 1;
        const int color2 = static_cast<int>(0.25 * myOpt.paletteSize);
        const int color3 = static_cast<int>(0.50 * myOpt.paletteSize);
        const int color4 = static_cast<int>(0.75 * myOpt.paletteSize);
        unsigned n = 0;

        for (y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for (n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if (z_re2 + z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + c_im + man_im;
                    z_re = z_re2 - z_im2 + c_re + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
                }
                if (insideSet)
                    setMap[x][y] = true;

                if (z_re > 0 && z_im > 0)
                    colorMap[x][y] = n + color1;
                else if (z_re <= 0 && z_im > 0)
                    colorMap[x][y] = n + color2;
                else if (z_re <= 0 && z_im < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderManowar::SpecialRender()
{
    // Creates fractal.
    double z_re, z_im;
    double c_re;

    for (y=ho; y<hf; y++)
    {
        double c_im = maxY - y * yFactor;
        for (x=wo; x<wf; x++)
        {
            double man_re = z_re = c_re = minX + x * xFactor;
            double man_im = z_im = c_im;
            bool broken = false;

            double distX = abs(c_re);
            double distY = abs(c_im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<maxIter; n++)
            {
                double z_re2 = z_re * z_re;
                double z_im2 = z_im * z_im;
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

                if (myOpt.orbitTrapMode)
                {
                    if(abs(z_im) < distY) distY = abs(z_im);
                    if(abs(z_re) < distX) distX = abs(z_re);
                }

                if (!broken)
                    iterations = n;
            }
            if (distX == 0)
                distX = 0.000001;
            if (distY == 0)
                distY = 0.000001;

            if (insideSet)
                setMap[x][y] = 1;

            if (myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = iterations;
        }
    }
}

