#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderManowar::RenderManowar()
{

}
void RenderManowar::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double c_re, c_im, temp_re, temp_im;
    double z_y_init;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
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
        unsigned n;
        double distance, distance1;
        double mu;
        double log2 = log(2.0);
        double loglog2 = log(log2);

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;
                insideSet = true;
                distance = 99;
                mu = (loglog2 - log(log(sqrt(4.0))))/log2 + 1;

                for(n=0; n<maxIter && insideSet; n++)
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
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = c_re = minX + x*xFactor;
                man_im = z_im = c_im = z_y_init;

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    z_re2 = z_re*z_re;
                    z_im2 = z_im*z_im;
                    if(z_re2 + z_im2 > 4)
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
                if(insideSet)
                    setMap[x][y] = true;

                if(z_re > 0 && z_im > 0)
                    colorMap[x][y] = n + color1;
                else if(z_re <= 0 && z_im > 0)
                    colorMap[x][y] = n + color2;
                else if(z_re <= 0 && z_im < 0)
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
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double c_re, c_im, temp_re, temp_im;

    double distX, distY;
    bool broken;

    for(y=ho; y<hf; y++)
    {
        c_im = maxY - y*yFactor;
        for(x=wo; x<wf; x++)
        {
            man_re = z_re = c_re = minX + x*xFactor;
            man_im = z_im = c_im;
            broken = false;

            distX = abs(c_re);
            distY = abs(c_im);

            insideSet = true;
            int iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                z_re2 = z_re*z_re;
                z_im2 = z_im*z_im;
                if(z_re2 + z_im2 > 4)
                {
                    insideSet = false;
                    broken = true;
                }
                temp_re = z_re;
                temp_im = z_im;
                z_im = 2*z_re*z_im + c_im + man_im;
                z_re = z_re2 - z_im2 + c_re + man_re;
                man_re = temp_re;
                man_im = temp_im;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z_im) < distY) distY = abs(z_im);
                    if(abs(z_re) < distX) distX = abs(z_re);
                }

                if(!broken) iterations = n;
            }
            if(distX == 0) distX = 0.000001;
            if(distY == 0) distY = 0.000001;

            if(insideSet)
                setMap[x][y] = 1;

            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = iterations;
        }
    }
}

