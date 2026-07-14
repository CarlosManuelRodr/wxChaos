#pragma once
#include "../../raster/RasterFractal.h"
#include "../../renderers/UserDefinedEscapeTimeRenderer.h"

/**
 * @class UserDefinedEscapeTime
 * @brief Escape-time fractal defined by a user muParserX expression.
 *
 * The expression computes the next z value and may use variables z/Z and c/C.
 * In Mandelbrot mode each pixel supplies c and starts z_0 = 0; in Julia mode
 * each pixel supplies z_0 and the UI Julia constant supplies c. Escape is
 * detected when |z|^2 exceeds bailout^2 from the formula options.
 */
class UserDefinedEscapeTime : public RasterFractal
{
    UserDefinedEscapeTimeRenderer* _myRender;
public:
    UserDefinedEscapeTime(unsigned int width, unsigned int height);
    ~UserDefinedEscapeTime() override;
    wxString GetName() const override { return "User Defined"; }

    void Render() override;
    void SetFormula(FormulaOptions formula) override;        ///< Sets user formula.
    void DrawOrbit() override;
    void PostRender() override;
};
