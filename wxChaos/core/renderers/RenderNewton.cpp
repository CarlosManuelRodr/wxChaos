#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderNewton::RenderNewton()
{

}
void RenderNewton::Render()
{
    double re, im;
    double tmp;
    double d;
    double re2, im2;
    double prev_re, prev_im;

    // Creates fractal.
    unsigned n;
    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            prev_re = re = minX + x*xFactor;
            prev_im = im = maxY - y*yFactor;

            for(n=0; n<maxIter; n++)
            {
                re2 = re*re;
                im2 = im*im;
                d = 3.0*((re2 - im2)*(re2 - im2) + 4.0*re2*im2);
                if(d == 0.0)
                    d = 0.000001;

                tmp = re;
                re = (2.0/3.0)*re + (re2 - im2)/d;
                im = (2.0/3.0)*im - 2.0*tmp*im/d;

                if((prev_re - minStep < re && prev_re + minStep > re) && (prev_im - minStep < im && prev_im + minStep > im))
                {
                    break;
                }
                else
                {
                    prev_re = re;
                    prev_im = im;
                }
            }
            if(re <= 0 && im >= 0)
                colorMap[x][y] = 1 + n;
            else if(re <= 0 && im < 0)
                colorMap[x][y] = 17 + n;
            else
                colorMap[x][y] = 37 + n;
        }
    }
}
void RenderNewton::SpecialRender()
{
    // Creates fractal.
    unsigned n;
    complex<double> z, z_prev;
    double distX, distY;
    double re, im;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            re = minX + x*xFactor;
            im = maxY - y*yFactor;
            z_prev = z = complex<double>(re, im);

            distX = abs(re);
            distY = abs(im);

            for(n=0; n<maxIter; n++)
            {
                z = z - (pow(z, 3) - complex<double>(1, 0))/(complex<double>(2, 0)*pow(z,2));

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;

                if(myOpt.orbitTrapMode)
                {
                    if(abs(z.imag()) < distY) distY = abs(z.imag());
                    if(abs(z.real()) < distX) distX = abs(z.real());
                }
            }
            if(myOpt.orbitTrapMode)
                colorMap[x][y] = static_cast<unsigned int>(n + log(1/distX) + log(1/distY));
            else
                colorMap[x][y] = n;
        }
    }
}
void RenderNewton::SetParams(double _minStep)
{
    minStep = _minStep;
}

