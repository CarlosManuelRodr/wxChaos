#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderJulia::RenderJulia() {}
void RenderJulia::Render()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    // Creates fractal.
    unsigned n;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = c_im;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + kImaginary;
                    Z_re = Z_re2 - Z_im2 + kReal;
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double temp_im;

        for(y=ho; y<hf; y++)
        {
            temp_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = temp_im;

                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im + kImaginary;
                    Z_re = Z_re2 - Z_im2 + kReal;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(Z_re, Z_im));
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

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    z = pow(z,2) + k;
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
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
    else if(myOpt.alg == RenderingAlgorithm::TriangleInequality)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);
        double tia_prev_x, tia_prev_y;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c_re = Z_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;
                distance = 0;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;

                    if(Z_re2 + Z_im2 > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(Z_re2 + Z_im2))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im;
                    Z_re = Z_re2 - Z_im2;

                    tia_prev_x = Z_re;
                    tia_prev_y = Z_im;

                    Z_re += kReal;
                    Z_im += kImaginary;

                    distance1 = distance;
                    if(n > 0) distance += TIA(Z_re, Z_im, c_re, c_im, tia_prev_x, tia_prev_y);
                }
                if(insideSet)
                    setMap[x][y] = true;

                distance1 = distance1/(n-1);
                distance = distance/n;
                colorMap[x][y] = static_cast<unsigned int>(abs(((mu*distance + (1-mu)*distance1)*700)));
            }
        }
    }
}
void RenderJulia::SpecialRender()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    double log2 = log(2.0);
    if(myOpt.orbitTrapMode)
    {
        // Creates fractal.
        complex<double> z, temp;
        complex<double> constant(kReal, kImaginary);
        double distX, distY;
        double re, im;
        bool broken;
        int out;

        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                re = minX + x*xFactor;
                im = maxY - y*yFactor;
                z = complex<double>(re, im);

                broken = false;
                distX = abs(re);
                distY = abs(im);
                insideSet = true;
                int iterations = 0;

                for(unsigned n=0; n<maxIter; n++)
                {
                    z = pow(z, 2) + constant;
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        if(!broken) temp = z;
                        insideSet = false;
                        broken = true;
                    }
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());

                    if(!broken) iterations = n;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;
                if(myOpt.smoothRender)
                {
                    if(!insideSet)
                    {
                        out = static_cast<int>(abs(4.0*(iterations -  log(log(pow(temp.real(),2)+pow(temp.imag(),2)))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                        if(out < 0) out = 0;
                        colorMap[x][y] = out;
                    }
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(abs(iterations + log(1/distX) + log(1/distY)));
            }
        }
    }
    if(myOpt.smoothRender && !(myOpt.orbitTrapMode))
    {
        unsigned n;
        int out;
        c_re = kReal;
        c_im = kImaginary;
        for(y=ho; y<hf; y++)
        {
            for(x=wo; x<wf; x++)
            {
                Z_re = minX + x*xFactor;
                Z_im = maxY - y*yFactor;
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }
                if(insideSet) setMap[x][y] = true;
                out = static_cast<int>(abs(4.0*(n -  log(log(Z_re2+Z_im2))/log2)));
                if(out < 0) out = 0;
                colorMap[x][y] = out;
            }
        }
    }
}

