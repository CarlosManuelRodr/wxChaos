#include "FixedPoint4Renderer.h"

FixedPoint4Renderer::FixedPoint4Renderer()
{
    _minStep = 0.01;
}
template<class Real>
void FixedPoint4Renderer::RenderConvergence()
{
    const Real minStep(_minStep);

    const auto renderPixel = [this, minStep](const Real& pixelRe, const Real& pixelIm)
    {
        PrecisionComplex<Real> z(pixelRe, pixelIm);
        PrecisionComplex<Real> zPrev = z;
        unsigned n;

        for (n = 0; n < _maxIterations; n++)
        {
            z = z * z;

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
void FixedPoint4Renderer::Render()
{
    if (_useHighPrecision)
        RenderConvergence<HighPrecisionReal>();
    else
        RenderConvergence<double>();
}
void FixedPoint4Renderer::SetParams(double minStep)
{
    _minStep = minStep;
}

