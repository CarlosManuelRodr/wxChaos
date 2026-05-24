#include <complex>
#include "RenderSinoidal.h"
#include "FractalUtils.h"
using namespace std;

RenderSinoidal::RenderSinoidal() {}
void RenderSinoidal::Render()
{
    // Creates fractal.
    unsigned n;
    complex<double> z;
    complex<double> k(kReal, kImaginary);
    bool insideSet;
    double c_im;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = k*sin(z);
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if (myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (n=0; n<maxIter && insideSet; n++)
                {
                    double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if (zNorm > maxIter)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    z = k*sin(z);

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if (insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if (myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1 = 1;
        int color2 = 0.25 * myOpt.paletteSize;
        int color3 = 0.50 * myOpt.paletteSize;
        int color4 = 0.75 * myOpt.paletteSize;

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for (n=0; n<maxIter; n++)
                {
                    z = k*sin(z);
                    if (z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    setMap[x][y] = true;

                if (z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if (z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if (z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderSinoidal::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> constant(kReal, kImaginary);

    for (y=ho; y<hf; y++)
    {
        for (x=wo; x<wf; x++)
        {
            double re = minX + x * xFactor;
            double im = maxY - y * yFactor;
            z = complex<double>(re, im);
            bool broken = false;

            double distanceX = abs(re);
            double distanceY = abs(im);

            bool insideSet = true;
            int iterations = 0;

            for (unsigned n=0; n<maxIter; n++)
            {
                z = constant*sin(z);
                if (z.real()*z.real() + z.imag()*z.imag() > maxIter)
                {
                    insideSet = false;
                    broken = true;
                }

                if (myOpt.orbitTrapMode)
                {
                    if (abs(z.imag()) < distanceY)
                        distanceY = abs(z.imag());
                    if (abs(z.real()) < distanceX)
                        distanceX = abs(z.real());
                }

                if(!broken)
                    iterations = n;
            }

            if (insideSet)
                setMap[x][y] = true;
            if (myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(abs(iterations + log(1/distanceX) + log(1/distanceY)));
            else
                colorMap[x][y] = iterations;
        }
    }
}

