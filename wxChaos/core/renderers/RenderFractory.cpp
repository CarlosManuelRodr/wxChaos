#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderFractory::RenderFractory()
{

}
void RenderFractory::Render()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, b, c;
    double c_im;
    bool insideSet;
    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
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

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {

                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        mu = (loglog2 - log(log(sqrt(z.real()*z.real()+ z.imag()*z.imag()))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;

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

        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                c = complex<double>(minX + x*xFactor, c_im);
                z = c;
                b = c - sin(c);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real()+ z.imag()*z.imag() > 4)
                    {
                        insideSet = false;
                        break;
                    }
                    b = c + b/c - z;
                    z = z*c + b/z;
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

