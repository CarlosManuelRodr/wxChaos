#pragma once
#include <complex>
#include "symbolic/UserFormulaSymbolicCompiler.h"
#include "../../raster/RasterFractal.h"
#include "../../renderers/UserDefinedNewtonRenderer.h"

/**
 * @brief Fractal definition for user-provided Newton-Raphson formulas.
 *
 * This class connects the formula dialog, fractal options panel, orbit drawing,
 * symbolic compilation, and the Newton renderer. It compiles f(z) and f'(z)
 * once when the formula changes, then passes muParserX-ready expressions to
 * the renderer for pixel iteration.
 */
class UserDefinedNewton : public RasterFractal
{
    UserDefinedNewtonRenderer* _myRender;
    CompiledUserFormula _compiledFormula;
    wxString _errorInfo;

    double _convergenceEpsilon;
    double _functionEpsilon;
    double _derivativeEpsilon;
    double _rootTolerance;
    double _escapeRadius;

    /**
     * @brief Outcome of tracing the selected point's Newton orbit.
     */
    struct OrbitResult
    {
        /** @brief true when the selected point converged to a root. */
        bool converged = false;

        /** @brief true when orbit tracing stopped because of a failure state. */
        bool failed = false;
    };

    /** @brief Report whether the current formula compiled successfully. */
    [[nodiscard]] bool HasCompiledFormula() const;

    /**
     * @brief Test whether both components of a complex value are finite.
     * @param value Complex value to inspect.
     * @return true when real and imaginary parts are finite.
     */
    static bool IsFinite(const std::complex<double>& value);

    /**
     * @brief Trace and draw the Newton orbit for the selected point.
     * @return Orbit convergence or failure state for post-draw coloring.
     */
    OrbitResult DrawNewtonOrbit();

    /** @brief Push the current compiled formula and options into the renderer. */
    void ApplyRendererState() const;

public:
    /**
     * @brief Create a user-defined Newton fractal with the requested viewport.
     */
    UserDefinedNewton(unsigned int width, unsigned int height);

    /** @brief Destroy the owned renderer. */
    ~UserDefinedNewton() override;

    /** @brief Return the display name used by menus and panels. */
    wxString GetName() const override { return "User Defined Newton-Raphson"; }

    /** @brief Render the current compiled formula. */
    void Render() override;

    /**
     * @brief Compile and store a new user formula.
     * @param formula Formula dialog options containing expression or equation text.
     */
    void SetFormula(FormulaOptions formula) override;

    /** @brief Draw the Newton orbit for the selected point. */
    void DrawOrbit() override;

    /** @brief Copy Newton-specific parameters from the options panel. */
    void CopyOptionFromPanel() override;

    /** @brief Report formula or render errors after rendering. */
    void PostRender() override;
};
