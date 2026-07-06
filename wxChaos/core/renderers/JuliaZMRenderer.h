#pragma once
#include "../RenderWorker.h"

/**
* @class JuliaZMRenderer
* @brief Renders power Julia samples for one pixel region.
*
* The renderer owns the formula trace for JuliaZN and dispatches the selected
* escape-time coloring algorithm through the shared Renderer paths.
*/
class JuliaZMRenderer : public RenderWorker
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
    JuliaZMRenderer();

    void Render() override;
    void SetParams(double m, double bailout);
};
