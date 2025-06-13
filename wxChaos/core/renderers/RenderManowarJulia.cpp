#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderManowarJulia::RenderManowarJulia()
{

}
void RenderManowarJulia::Render()
{
    // Creates fractal.
    unsigned n;
    bool insideSet;
    double z_re, z_im, z_re2, z_im2, man_re, man_im;
    double temp_re, temp_im;
    double z_y_init;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;
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
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;
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
        double temp_im;

        for(y=ho; y<hf; y++)
        {
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;

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
                        if(n > 0) insideSet = false;
                    }
                    temp_re = z_re;
                    temp_im = z_im;
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
                    man_re = temp_re;
                    man_im = temp_im;

                    distance1 = distance;
                    distance = minVal(distance, gaussianIntDist(z_re, z_im));
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
            z_y_init = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                man_re = z_re = minX + x*xFactor;
                man_im = z_im = z_y_init;
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
                    z_im = 2*z_re*z_im + kImaginary + man_im;
                    z_re = z_re2 - z_im2 + kReal + man_re;
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
void RenderManowarJulia::SpecialRender()
{
    // Creates fractal.
    complex<double> z;
    complex<double> man;
    complex<double> temp;
    complex<double> constant;
    bool insideSet;
    int iterations;

    double distX, distY;
    double re, im;
    bool broken;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            man = z = complex<double>(re, im);
            constant = complex<double>(kReal, kImaginary);
            broken = false;

            distX = abs(re);
            distY = abs(im);

            insideSet = true;
            iterations = 0;

            for(unsigned n=0; n<maxIter; n++)
            {
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    broken = true;
                }

                temp = z;
                z = pow(z, 2) + man + constant;
                man = temp;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }

                if(!broken) iterations = n;
            }
            if(insideSet)
                setMap[x][y] = true;

            if(distX == 0) distX = 0.000001;
            if(distY == 0) distY = 0.000001;

            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(iterations + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = iterations;
        }
    }
}

