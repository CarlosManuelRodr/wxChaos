#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderMagnet::RenderMagnet()
{

}
void RenderMagnet::Render()
{
    // Creates fractal.
    complex<double> z;
    complex<double> c;
    unsigned n;
    bool insideSet;
    double c_im;

    if(myOpt.alg == RenderingAlgorithm::EscapeTime)
    {
        for(y=ho; y<hf; y++)
        {
            c_im = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                z = complex<double>(0, 0);
                c = complex<double>(minX + x*xFactor, c_im);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {

                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
                }
                if(insideSet) setMap[x][y] = true;
                colorMap[x][y] = n;
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
                z = complex<double>(0, 0);
                c = complex<double>(minX + x*xFactor, c_im);
                insideSet = true;

                for(n=0; n<maxIter; n++)
                {
                    if(z.real()*z.real() + z.imag()*z.imag() > maxIter)
                    {
                        insideSet = false;
                        break;
                    }
                    z = pow((pow(z, 2) + c - complex<double>(1.0, 0.0))/(complex<double>(2.0, 0.0)*z + c - complex<double>(2.0, 0.0)), 2.0);
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

