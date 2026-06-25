#pragma once

#include <string>

/**
 * @brief muParserX-ready representation of a user supplied Newton formula.
 *
 * The symbolic compiler normalizes the user's input into a function expression
 * and derives the corresponding derivative expression. Both strings are
 * intended to be compiled by muParserX, not evaluated by SymEngine during
 * rendering.
 */
struct CompiledUserFormula
{
    /** @brief Normalized function expression f(z). */
    std::string expression;

    /** @brief Symbolically generated derivative expression f'(z). */
    std::string derivativeExpression;
};

/**
 * @brief Converts user formula text into compiled Newton formula source.
 *
 * This class owns the one-time symbolic work for user-defined Newton fractals:
 * equation normalization, SymEngine parsing, symbol validation, symbolic
 * differentiation, and conversion back into muParserX-compatible syntax.
 */
class UserFormulaSymbolicCompiler
{
public:
    /**
     * @brief Compile a Newton formula from a user expression or equation.
     * @param input User text such as `z^3 - 1` or `z^3 - 1 = 0`.
     * @param output Receives the normalized function and derivative strings.
     * @param error Receives a user-facing error message on failure.
     * @return true when both expressions were generated successfully.
     */
    static bool CompileNewtonFormula(const std::string& input, CompiledUserFormula& output, std::string& error);

private:
    /**
     * @brief Convert optional single-equals equation syntax to `lhs - rhs`.
     * @param input User expression or equation.
     * @param error Receives an explanation when the equation syntax is invalid.
     * @return Normalized expression text, or an empty string on failure.
     */
    static std::string NormalizeEquation(const std::string& input, std::string& error);

    /**
     * @brief Convert SymEngine expression text into muParserX syntax.
     * @param expression Expression emitted by SymEngine.
     * @return Expression text accepted by the existing muParserX evaluators.
     */
    static std::string ToMuParserExpression(const std::string& expression);
};
