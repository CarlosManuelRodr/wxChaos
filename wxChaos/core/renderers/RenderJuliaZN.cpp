#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderJuliaZN::RenderJuliaZN()
{
    n = 0;
    bailout = 0;
}
void RenderJuliaZN::Render()
{
    double c_im;
    bool insideSet;
    unsigned i;
    double squaredBail = bailout*bailout;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double squaredBail = bailout*bailout;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = i;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double zNorm;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(i=0; i<maxIter && insideSet; i++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(i > 0) insideSet = false;
                    }
                    z = pow(z,n) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*myOpt.paletteSize)));
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::EscapeAngle)
    {
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;
        complex<double> z, k;
        k = complex<double>(kReal, kImaginary);
        double squaredBail = bailout*bailout;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                if(z.real() > 0 && z.imag() > 0)
                    colorMap[x][y] = n + color1;
                else if(z.real() <= 0 && z.imag() > 0)
                    colorMap[x][y] = n + color2;
                else if(z.real() <= 0 && z.imag() < 0)
                    colorMap[x][y] = n + color3;
                else
                    colorMap[x][y] = n + color4;
            }
        }
    }
}
void RenderJuliaZN::SpecialRender()
{
    double Z_re2;
    double Z_im2;
    double c_im;
    bool insideSet;
    complex<double> z, k;
    k = complex<double>(kReal, kImaginary);
    double squaredBail = bailout*bailout;
    double log2 = log(2.0);
    unsigned i;
    int out;

    if(myOpt.orbitTrapMode)
    {
        double distX, distY;
        int iterations;
        double bailFourPower = squaredBail*squaredBail;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                distX = abs(z.real());
                distY = abs(z.imag());

                insideSet = true;
                iterations = 0;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z, n) + k;
                    Z_re2 = z.real()*z.real();
                    Z_im2 = z.imag()*z.imag();
                    if(Z_re2 + Z_im2 > squaredBail)
                    {
                        insideSet = false;
                        if(Z_re2 + Z_im2 > bailFourPower)
                        {
                            if(abs(z.imag()) < distY) distY = abs(z.imag());
                            if(abs(z.real()) < distX) distX = abs(z.real());
                            break;
                        }
                    }
                    else iterations = i;

                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;

                if(insideSet)
                    setMap[x][y] = true;
                if(myOpt.smoothRender)
                {
                    out = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    if(out < 0) out = 0;
                    if(!insideSet)
                        colorMap[x][y] = out;
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    else if(myOpt.smoothRender)
    {
        double squaredBail = bailout*bailout;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(i=0; i<maxIter; i++)
                {
                    z = pow(z,n) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                }
                if(insideSet)
                    setMap[x][y] = true;

                out = static_cast<int>(abs(4.0*(i -  log(log(z.real()*z.real()+z.imag()*z.imag()))/log2)));
                if(out < 0) out = 0;
                colorMap[x][y] = out;
            }
        }
    }
}
void RenderJuliaZN::SetParams(int _n, double _bailout)
{
    n = _n;
    bailout = _bailout;
}

