#pragma once

#include <complex>
#include <string>
#include <wx/string.h>
#include "utils/NewtonRootRegistry.h"
#include "../Renderer.h"

/**
 * @brief Renders Newton-Raphson basins for a compiled user formula.
 *
 * The renderer evaluates precompiled muParserX expressions for f(z) and f'(z)
 * during iteration. It does not call SymEngine while rendering. Roots are
 * discovered numerically as pixels converge and are colored by registry id.
 */
class UserDefinedNewtonRenderer : public Renderer
{
    std::string _functionExpression;
    std::string _derivativeExpression;
    wxString _errorInfo;
    NewtonRootRegistry _rootRegistry;

    double _convergenceEpsilon;
    double _functionEpsilon;
    double _derivativeEpsilon;
    double _rootTolerance;
    double _escapeRadius;

    /**
     * @brief Result of iterating a single pixel through Newton's method.
     */
    struct IterationResult
    {
        /** @brief true when the pixel converged to a numerical root. */
        bool converged = false;

        /** @brief Number of Newton steps performed. */
        unsigned int iterations = 0;

        /** @brief Final Newton value or last finite iterate. */
        std::complex<double> finalZ;

        /** @brief Magnitude of the final Newton step. */
        double finalStep = 0.0;

        /** @brief Magnitude of the final function residual. */
        double finalResidual = 0.0;
    };

    /** @brief Check whether both muParserX source expressions are available. */
    [[nodiscard]] bool HasCompiledFormula() const;

    /**
     * @brief Test whether both components of a complex value are finite.
     * @param value Complex value to inspect.
     * @return true when real and imaginary parts are finite.
     */
    static bool IsFinite(const std::complex<double>& value);

    /**
     * @brief Compute the deterministic basin color value for one pixel.
     * @param rootId Numerical root id assigned by the registry.
     * @param result Iteration data used for iteration and convergence shading.
     * @return Scalar color value consumed by the inherited coloring path.
     */
    [[nodiscard]] double ColorValue(unsigned int rootId, const IterationResult& result) const;

    /** @brief Fill the output with the configured set color after a fatal error. */
    void FillFailedRegion() const;

public:
    /** @brief Create a renderer with default Newton thresholds. */
    UserDefinedNewtonRenderer();

    /**
     * @brief Provide muParserX-ready source for f(z) and f'(z).
     * @param functionExpression Normalized function expression.
     * @param derivativeExpression Symbolically generated derivative expression.
     */
    void SetCompiledFormula(const std::string& functionExpression, const std::string& derivativeExpression);

    /**
     * @brief Store a formula compilation error to be reported after rendering.
     * @param errorInfo User-facing error message.
     */
    void SetFormulaError(const wxString& errorInfo);

    /**
     * @brief Update the Newton stopping thresholds and root tolerance.
     */
    void SetParams(double convergenceEpsilon, double functionEpsilon, double derivativeEpsilon,
                   double rootTolerance, double escapeRadius);

    /** @brief Render the current view using Newton-Raphson iteration. */
    void Render() override;

    /** @brief Clear any stored formula or render error. */
    void ClearErrorInfo();

    /** @brief Return the latest formula or render error. */
    [[nodiscard]] wxString GetErrorInfo() const;

    /** @brief Report whether an error is currently stored. */
    [[nodiscard]] bool IsThereError() const;
};
