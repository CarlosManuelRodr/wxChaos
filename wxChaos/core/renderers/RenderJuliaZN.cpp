#include <complex>
#include "RenderJuliaZN.h"
#include "FractalUtils.h"
using namespace std;

RenderJuliaZN::RenderJuliaZN()
{
    _n = 0;
    _bailout = 0;
}
void RenderJuliaZN::Render()
{
    double c_im;
    bool insideSet;
    unsigned i;

    // Creates fractal.
    if (myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        complex<double> k = complex<double>(kReal, kImaginary);
        double squaredBail = _bailout*_bailout;
        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                complex<double> z = complex<double>(minX + x * xFactor, c_im);
                insideSet = true;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + k;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
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
        complex<double> z;
        complex<double> k = complex<double>(kReal, kImaginary);
        double distance1 = 0;
        const double log2 = log(2.0);
        const double loglog2 = log(log2);
        const double squaredBail = _bailout*_bailout;

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                double distance = 99;
                double mu = (loglog2 - log(log(sqrt(4.0)))) / log2 + 1;

                for (i=0; i<maxIter && insideSet; i++)
                {
                    const double zNorm = z.real() * z.real() + z.imag() * z.imag();
                    if(zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(i > 0) insideSet = false;
                    }
                    z = pow(z,_n) + k;

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
        constexpr int color1 = 1;
        const int color2 = 0.25 * myOpt.paletteSize;
        const int color3 = 0.50 * myOpt.paletteSize;
        const int color4 = 0.75 * myOpt.paletteSize;
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double squaredBail = _bailout*_bailout;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = _n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = _n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = _n + color3;
                else
                    colorMap[x][y] = _n + color4;
            }
        }
    }
}
void RenderJuliaZN::SpecialRender()
{
    double c_im;
    bool insideSet;
    complex<double> z;
    const complex<double> k = complex<double>(kReal, kImaginary);
    const double log2 = log(2.0);
    unsigned i;
    int out;

    if (myOpt.orbitTrapMode)
    {
        const double squaredBail = _bailout*_bailout;
        double Z_im2 = 0;
        double Z_re2 = 0;
        const double bailFourPower = squaredBail*squaredBail;

        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);

                double distX = abs(z.real());
                double distY = abs(z.imag());

                insideSet = true;
                int iterations = 0;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z, _n) + k;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if (Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if (Z_re2 + Z_im2 > bailFourPower)
                        {
                            if(abs(z.imag()) < distY) distY = abs(z.imag());
                            if(abs(z.real()) < distX) distX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if(abs(z.imag()) < distY)
                        distY = abs(z.imag());
                    if(abs(z.real()) < distX)
                        distX = abs(z.real());
                }
                if (distX == 0)
                    distX = 0.000001;
                if (distY == 0)
                    distY = 0.000001;

                if (insideSet)
                    setMap[x][y] = true;
                if (myOpt.smoothRender)
                {
                    out = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    if (out < 0)
                        out = 0;
                    if (!insideSet)
                        colorMap[x][y] = out;
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    else if (myOpt.smoothRender)
    {
        double squaredBail = _bailout*_bailout;
        for (y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for (x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for (i=0; i<maxIter; i++)
                {
                    z = pow(z,_n) + k;
                    if (z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if (insideSet)
                    setMap[x][y] = true;

                out = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
                if(out < 0) out = 0;
                colorMap[x][y] = out;
            }
        }
    }
}
void RenderJuliaZN::SetParams(const int n, const double bailout)
{
    _n = n;
    _bailout = bailout;
}

