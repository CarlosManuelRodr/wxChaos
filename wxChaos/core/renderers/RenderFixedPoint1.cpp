#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderFixedPoint1::RenderFixedPoint1()
{
    minStep = 0.01;
}
void RenderFixedPoint1::Render()
{
    complex<double> z;
    complex<double> z_prev;
    unsigned n;

    for(y=ho; y<hf; y++)
    {
        for(x=wo; x<wf; x++)
        {
            z_prev = z = complex<double>(minX + x*xFactor, maxY - y*yFactor);

            for(n=0; n<maxIter; n++)
            {
                z = sin(z);

                if((z_prev.real() - minStep < z.real() && z_prev.real() + minStep > z.real())
                    && (z_prev.imag() - minStep < z.imag() && z_prev.imag() + minStep > z.imag()))
                    break;
                else
                    z_prev = z;
            }
            if(z.real() > 0) colorMap[x][y] = 1 + n;
            else colorMap[x][y] = 30 + n;
        }
    }
}
void RenderFixedPoint1::SetParams(double _minStep)
{
    minStep = _minStep;
}

