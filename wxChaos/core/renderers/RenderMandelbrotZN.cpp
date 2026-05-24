#include <complex>
#include "RenderMandelbrotZN.h"
#include "FractalUtils.h"
using namespace std;

RenderMandelbrotZN::RenderMandelbrotZN()
{
    _n = 0;
    _bailout = 0;
}
void RenderMandelbrotZN::Render()
{
    double c_im;
    bool insideSet;

    // Creates fractal.
    unsigned i;
    complex<double> z, c;
    const double squaredBail = _bailout*_bailout;
    if (myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = i;
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
                z = complex<double>(0,0);
                c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (i=0; i<maxIter && insideSet; i++)
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
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if (myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        // ReSharper disable once CppTooWideScope
        constexpr int color1 = 1;
        const int color2 = 0.25 * myOpt.paletteSize;
        const int color3 = 0.50 * myOpt.paletteSize;
        const int color4 = 0.75 * myOpt.paletteSize;

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    setMap[x][y] = true;

                if (z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = i + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = i + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = i + color3;
                else
                    colorMap[x][y] = i + color4;
            }
        }
    }
}
void RenderMandelbrotZN::SpecialRender()
{
    double c_im;
    bool insideSet;
    const double squaredBail = _bailout*_bailout;
    const double log2 = log(2.0);
    unsigned i;

    if (myOpt.orbitTrapMode)
    {
        double Z_im2 = 0;
        double Z_re2 = 0;

        // Creates fractal.
        complex<double> c;
        const double bailFourPower = squaredBail * squaredBail;

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                complex<double> z = c = complex<double>(minX + x * xFactor, c_im);

                double distanceX = abs(z.real());
                double distanceY = abs(z.imag());

                insideSet = true;
                int iterations = 0;

                for (i=0; i<maxIter; i++)
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
                    setMap[x][y] = true;

                if (myOpt.smoothRender)
                {
                    if (!insideSet)
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distanceX) + log(1/distanceY))));
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distanceX) + log(1/distanceY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distanceX) + log(1/distanceY)));
            }
        }
    }
    else if (myOpt.smoothRender)
    {
        // Creates fractal.
        complex<double> z, c;
        double squaredBail = _bailout*_bailout;
        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + c;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
            }
        }
    }
}
void RenderMandelbrotZN::SetParams(int n, double bailout)
{
    _n = n;
    _bailout = bailout;
}

