#include "FixedPoint3Renderer.h"

FixedPoint3Renderer::FixedPoint3Renderer()
{
    _minStep = 0.01;
}
template<class Real>
void FixedPoint3Renderer::RenderConvergence()
{
    const Real minStep(_minStep);

    const auto renderPixel = [this, minStep](const Real& pixelRe, const Real& pixelIm)
    {
        PrecisionComplex<Real> z(pixelRe, pixelIm);
        PrecisionComplex<Real> zPrev = z;
        unsigned n;

        for (n = 0; n < _maxIterations; n++)
        {
            z = ComplexTan(z);

            if ((zPrev.re - minStep < z.re && zPrev.re + minStep > z.re)
                && (zPrev.im - minStep < z.im && zPrev.im + minStep > z.im))
                break;

            zPrev = z;
        }

        _colorMap[_x][_y] = (z.re > Real(0) ? 1 : 30) + n;
    };

    if constexpr (std::is_same_v<Real, double>)
        RenderPixels(renderPixel);
    else
        RenderPixelsPrecise(renderPixel);
}
void FixedPoint3Renderer::Render()
{
    if (_useHighPrecision)
        RenderConvergence<HighPrecisionReal>();
    else
        RenderConvergence<double>();
}
void FixedPoint3Renderer::SetParams(double minStep)
{
    _minStep = minStep;
}

