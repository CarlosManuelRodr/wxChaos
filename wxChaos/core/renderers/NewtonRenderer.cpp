#include <cmath>
#include "NewtonRenderer.h"

NewtonRenderer::NewtonRenderer()
{
    minStep = 0.001;
}

template<class Real>
void NewtonRenderer::RenderConvergenceTest()
{
    const Real step(minStep);
    const Real zero(0);

    const auto renderPixel = [this, step, zero](const Real& pixelRe, const Real& pixelIm)
    {
        Real re = pixelRe;
        Real im = pixelIm;
        Real prevRe = re;
        Real prevIm = im;
        unsigned n;

        for (n = 0; n < _maxIterations; n++)
        {
            const Real re2 = re * re;
            const Real im2 = im * im;
            Real d = Real(3) * ((re2 - im2) * (re2 - im2) + Real(4) * re2 * im2);
            if (d == zero)
                d = Real(0.000001);

            const Real tmp = re;
            re = (Real(2) / Real(3)) * re + (re2 - im2) / d;
            im = (Real(2) / Real(3)) * im - Real(2) * tmp * im / d;

            if ((prevRe - step < re && prevRe + step > re) && (prevIm - step < im && prevIm + step > im))
                break;

            prevRe = re;
            prevIm = im;
        }

        if (re <= zero && im >= zero)
            _colorMap[_x][_y] = 1 + n;
        else if (re <= zero && im < zero)
            _colorMap[_x][_y] = 17 + n;
        else
            _colorMap[_x][_y] = 37 + n;
    };

    if constexpr (std::is_same_v<Real, double>)
        RenderPixels(renderPixel);
    else
        RenderPixelsPrecise(renderPixel);
}
template<class Real>
void NewtonRenderer::RenderConvergenceTestWithOrbitTrap()
{
    const Real step(minStep);
    const PrecisionComplex<Real> one(Real(1), Real(0));
    const PrecisionComplex<Real> two(Real(2), Real(0));

    const auto renderPixel = [this, step, one, two](const Real& pixelRe, const Real& pixelIm)
    {
        PrecisionComplex<Real> z(pixelRe, pixelIm);
        PrecisionComplex<Real> zPrev = z;
        Real distX = RealAbs(pixelRe);
        Real distY = RealAbs(pixelIm);
        unsigned n;

        for (n = 0; n < _maxIterations; n++)
        {
            z = z - (ComplexPow(z, 3) - one) / (two * ComplexPow(z, 2));

            if ((zPrev.re - step < z.re && zPrev.re + step > z.re)
                && (zPrev.im - step < z.im && zPrev.im + step > z.im))
                break;

            zPrev = z;

            if (RealAbs(z.im) < distY)
                distY = RealAbs(z.im);
            if (RealAbs(z.re) < distX)
                distX = RealAbs(z.re);
        }

        if constexpr (std::is_same_v<Real, double>)
            _colorMap[_x][_y] = ToColorMapValue(n + std::log(1 / distX) + std::log(1 / distY));
        else
            _colorMap[_x][_y] = ToColorMapValue(n + ToDouble(log(Real(1) / distX) + log(Real(1) / distY)));
    };

    if constexpr (std::is_same_v<Real, double>)
        RenderPixels(renderPixel);
    else
        RenderPixelsPrecise(renderPixel);
}
void NewtonRenderer::ConvergenceTestRender()
{
    if (_useHighPrecision)
        RenderConvergenceTest<HighPrecisionReal>();
    else
        RenderConvergenceTest<double>();
}
void NewtonRenderer::ConvergenceTestWithOrbitTrapRender()
{
    if (_useHighPrecision)
        RenderConvergenceTestWithOrbitTrap<HighPrecisionReal>();
    else
        RenderConvergenceTestWithOrbitTrap<double>();
}
void NewtonRenderer::Render()
{
    if (_myOpt.orbitTrapMode)
        ConvergenceTestWithOrbitTrapRender();
    else
        ConvergenceTestRender();
}

void NewtonRenderer::SetParams(double _minStep)
{
    minStep = _minStep;
}

