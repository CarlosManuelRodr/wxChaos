#pragma once
#include "../Fractal.h"
#include "../renderers/UserDefinedFixedPointRenderer.h"

/**
 * @class UserDefinedFixedPoint
 * @brief Fixed-point convergence map defined by a user muParserX expression.
 *
 * The expression computes the next z value and may use variables z/Z. Each
 * pixel supplies z_0. Iteration stops when the real and imaginary parts both
 * change by less than the configured minimum step, or when max iterations is
 * reached.
 */
class UserDefinedFixedPoint : public Fractal
{
    UserDefinedFixedPointRenderer* _myRender;
    double _minStep;
public:
    UserDefinedFixedPoint(unsigned int width, unsigned int height);
    ~UserDefinedFixedPoint() override;
    wxString GetName() const override { return wxT("User Defined Fixed Point"); }

    void Render() override;
    void SetFormula(FormulaOptions formula) override;        ///< Sets user formula.
    void DrawOrbit() override;
    void CopyOptFromPanel() override;
    void PostRender() override;
};
