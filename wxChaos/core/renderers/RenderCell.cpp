#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderCell::RenderCell()
{
    bailout = 1.0;
}
void RenderCell::Render()
{
    // Creates fractal.
    unsigned n;
    double squaredBail = bailout*bailout;
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
                    if(z.real()*z.real()+ z.imag()*z.imag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                    b /= c;
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
        unsigned n;
        double distance, distance1;
        double mu, norm;
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
                    norm = z.real()*z.real()+ z.imag()*z.imag();
                    if(norm > squaredBail)
                    {
                        mu = (loglog2 - log(log(sqrt(norm))))/log2 + 1;
                        if(n > 0) insideSet = false;
                    }
                    b /= c;
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
}
void RenderCell::SetParams(double _bailout)
{
    bailout = _bailout;
}

