#pragma once

#include <string>
#include <symengine/basic.h>

/**
 * @brief muParserX-ready representation of a user-supplied Newton formula.
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
 * @brief Converts user formula text into a compiled Newton formula source.
 *
 * This class owns the one-time symbolic work for user-defined Newton fractals:
 * equation normalization, SymEngine parsing, symbol validation, symbolic
 * differentiation, and conversion back into muParserX-compatible syntax.
 */
class UserFormulaSymbolicCompiler
{
    /**
     * @brief Check whether a SymEngine node type is unsupported by the Newton compiler.
     * @param typeName SymEngine type-code name.
     * @return true when the construct should be rejected before muParserX compilation.
     */
    static bool ContainsUnsupportedTypeName(const std::string& typeName);

    /**
     * @brief Gets the user-facing function name for an unsupported SymEngine function node.
     * @param typeName SymEngine type-code name.
     * @return Lowercase function name, or an empty string when the type is not a known function.
     */
    static std::string GetUnsupportedFunctionName(const std::string& typeName);

    /**
     * @brief Validate symbols and symbolic constructs in a parsed expression tree.
     * @param expression SymEngine expression tree to inspect.
     * @param error Receives a user-facing validation error on failure.
     * @return true when the tree only uses supported Newton formula constructs.
     */
    static bool ValidateExpressionTree(const SymEngine::RCP<const SymEngine::Basic>& expression, std::string& error);

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

public:
    /**
     * @brief Compile a Newton formula from a user expression or equation.
     * @param input User text such as `z^3 - 1` or `z^3 - 1 = 0`.
     * @param output Receives the normalized function and derivative strings.
     * @param error Receives a user-facing error message on failure.
     * @return true when both expressions were generated successfully.
     */
    static bool CompileNewtonFormula(const std::string& input, CompiledUserFormula& output, std::string& error);
};
