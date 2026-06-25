#include <doctest/doctest.h>

#include "UserFormulaSymbolicCompiler.h"

namespace
{
    CompiledUserFormula CompileOk(const std::string& expression)
    {
        CompiledUserFormula output;
        std::string error;
        REQUIRE(UserFormulaSymbolicCompiler::CompileNewtonFormula(expression, output, error));
        CHECK(error.empty());
        CHECK_FALSE(output.expression.empty());
        CHECK_FALSE(output.derivativeExpression.empty());
        CHECK(output.expression.find("**") == std::string::npos);
        CHECK(output.derivativeExpression.find("**") == std::string::npos);
        return output;
    }
}

TEST_CASE("Newton symbolic compiler derives polynomial expression")
{
    const CompiledUserFormula output = CompileOk("z^3 - 1");
    CHECK(output.derivativeExpression.find("z^2") != std::string::npos);
}

TEST_CASE("Newton symbolic compiler accepts equation form")
{
    const CompiledUserFormula output = CompileOk("z^3 - 1 = 0");
    CHECK(output.expression.find("z^3") != std::string::npos);
}

TEST_CASE("Newton symbolic compiler accepts common holomorphic functions")
{
    CompileOk("sin(z) - 0.5*z");
    CompileOk("exp(z) - 1");
}

TEST_CASE("Newton symbolic compiler rejects unknown symbols")
{
    CompiledUserFormula output;
    std::string error;
    CHECK_FALSE(UserFormulaSymbolicCompiler::CompileNewtonFormula("x^2 - 1", output, error));
    CHECK(error == "Unknown symbol: x. Use z as the complex variable.");
}

TEST_CASE("Newton symbolic compiler rejects unsupported functions")
{
    CompiledUserFormula output;
    std::string error;
    CHECK_FALSE(UserFormulaSymbolicCompiler::CompileNewtonFormula("gamma(z)", output, error));
    CHECK(error.find("Unsupported function: gamma") != std::string::npos);
}
