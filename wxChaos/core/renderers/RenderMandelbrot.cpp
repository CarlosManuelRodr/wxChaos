#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderMandelbrot::RenderMandelbrot()
{
    buddhaRandomP = 0;
    bd = 0;
}
void RenderMandelbrot::Render()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    // Creates fractal.
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        unsigned n;
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
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

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    else if(myOpt.alg == RenderingAlgorithm::GaussianInt)
    {
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
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
                        insideSet = false;
                    }

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

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
        unsigned n;
        int color1, color2, color3, color4;
        color1 = 1;
        color2 = 0.25*myOpt.paletteSize;
        color3 = 0.50*myOpt.paletteSize;
        color4 = 0.75*myOpt.paletteSize;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
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
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet)
                    setMap[x][y] = true;

                if(Z_re > 0 && Z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(Z_re <= 0 && Z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(Z_re <= 0 && Z_im < 0)
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
                Z_re = Z_im = 0;
                c_re = minX + x*xFactor;
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

                    Z_re += c_re;
                    Z_im += c_im;

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
void RenderMandelbrot::SpecialRender()
{
    double c_im;
    double c_re;
    double Z_re;
    double Z_im;
    double Z_re2;
    double Z_im2;
    bool insideSet;

    if(myOpt.orbitTrapMode)
    {
        // Creates fractal.
        double distX, distY;
        int iterations;
        double log2 = log(2.0);

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
                Z_im = c_im;
                insideSet = true;

                distX = abs(Z_re);
                distY = abs(Z_im);

                insideSet = true;
                iterations = 0;

                for(unsigned n=0; n<maxIter; n++)
                {
                    Z_re2 = Z_re*Z_re;
                    Z_im2 = Z_im*Z_im;
                    if(Z_re2 + Z_im2 > 4)
                    {
                        insideSet = false;
                        if(Z_re2 + Z_im2 > 16)
                        {
                            if(abs(Z_im) < distY) distY = abs(Z_im);
                            if(abs(Z_re) < distX) distX = abs(Z_re);
                            break;
                        }
                    }
                    else iterations = n;

                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;

                    if(abs(Z_im) < distY) distY = abs(Z_im);
                    if(abs(Z_re) < distX) distX = abs(Z_re);
                }
                if(distX == 0) distX = 0.000001;
                if(distY == 0) distY = 0.000001;

                if(insideSet)
                    setMap[x][y] = true;

                if(myOpt.smoothRender)
                {
                    if(!insideSet)
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations -  log(log(Z_re2+Z_im2))/log2) + 4.0*(log(1/distX) + log(1/distY))));
                    else
                        colorMap[x][y] = static_cast<int>(abs(4.0*(iterations + 4.0*(log(1/distX) + log(1/distY)))));
                }
                else
                    colorMap[x][y] = static_cast<int>(iterations + log(1/distX) + log(1/distY));
            }
        }
    }
    if(myOpt.smoothRender && !(myOpt.orbitTrapMode))
    {
        unsigned n;
        double log2 = log(2.0);
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                Z_re = c_re = minX + x*xFactor;
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
                    Z_im = 2*Z_re*Z_im + c_im;
                    Z_re = Z_re2 - Z_im2 + c_re;
                }

                if(insideSet) setMap[x][y] = true;
                colorMap[x][y] = static_cast<unsigned int>(abs(4.0*(n -  log(log(Z_re2+Z_im2))/log2)));
            }
        }
    }
}
void RenderMandelbrot::SetBuddhaRandomP(int n)
{
    buddhaRandomP = n;
}
int RenderMandelbrot::AskProgress()
{
    if(!stopped)
    {
        threadProgress = static_cast<int>(100.0 * ((double)(y + 1 - oldHo) / (double)(hf - oldHo)));
    }
    return threadProgress;
}

