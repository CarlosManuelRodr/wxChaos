#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderSierpTriangle::RenderSierpTriangle()
{

}
void RenderSierpTriangle::Render()
{
    complex<double> z;
    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            bool insideSet = true;
            int iterations = 0;
            for(unsigned n=0; n<maxIter; n++)
            {
                if(z.real()*z.real() + z.imag()*z.imag() > 4)
                {
                    insideSet = false;
                    break;
                }

                if(z.imag() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(0, 1);
                else if(z.real() > 0.5)
                    z = complex<double>(2, 0)*z - complex<double>(1, 0);
                else
                    z = complex<double>(2, 0)*z;

                iterations = n;
            }
            if(insideSet)
                setMap[x][y] = true;

            colorMap[x][y] = iterations;
        }
    }
}

