#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderMedusa::RenderMedusa()
{

}
void RenderMedusa::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    complex<double> z, k;
    k = complex<double>(kReal, kImaginary);
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
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
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
        double zNorm;
        double c_im;

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    zNorm = z.real()*z.real() + z.imag()*z.imag();
                    if(zNorm > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(zNorm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    z = pow(z, 1.5) + k;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z.real(), z.imag()));
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
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

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow(z, 1.5) + k;
                }
                if(insideSet)
                {
                    setMap[x][y] = true;
                }
                if(z.real() > 0 && z.imag() > 0)
                {
                    colorMap[x][y] = n + color1;
                }
                else if(z.real() <= 0 && z.imag() > 0)
                {
                    colorMap[x][y] = n + color2;
                }
                else if(z.real() <= 0 && z.imag() < 0)
                {
                    colorMap[x][y] = n + color3;
                }
                else
                {
                    colorMap[x][y] = n + color4;
                }
            }
        }
    }
}
void RenderMedusa::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> constant(kReal, kImaginary);
    double distX, distY;
    double re, im;
    bool broken;
    bool insideSet;

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
                z = pow(z, 1.5) + constant;
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }

                if(!broken) iterations = n;
            }
            if(insideSet)
            {
                setMap[x][y] = true;
            }
            if(myOpt.orbitTrapMode)
            {
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            }
            else
            {
                colorMap[x][y] = iterations;
            }
        }
    }
}

