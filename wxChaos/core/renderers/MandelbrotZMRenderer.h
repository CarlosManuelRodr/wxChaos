#pragma once
#include "../RasterRenderWorker.h"

/**
* @class MandelbrotZMRenderer
* @brief Renders Multibrot samples for one pixel region.
*
* The renderer owns the formula trace for MandelbrotZN and dispatches the
* selected escape-time coloring algorithm through the shared Renderer paths.
*/
class MandelbrotZMRenderer : public RasterRenderWorker
{
    double _m;
    int _integerM;
    bool _useIntegerExponent;
    double _bailout;

    static bool IsIntegerExponent(double exponent, int& integerExponent);

    template<class Real>
    PrecisionComplex<Real> Power(const PrecisionComplex<Real>& z) const;

    template<class Real, class MeasurePoint>
    Point TracePoint(const Real& pixelRe, const Real& pixelIm, MeasurePoint measure) const;

public:
    MandelbrotZMRenderer();

    void Render() override;
    void SetParams(double m, double bailout);
};
