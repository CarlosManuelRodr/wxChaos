#pragma once

#include <string>

struct CompiledUserFormula
{
    std::string expression;
    std::string derivativeExpression;
};

class UserFormulaSymbolicCompiler
{
public:
    static bool CompileNewtonFormula(const std::string& input, CompiledUserFormula& output, std::string& error);

private:
    static std::string NormalizeEquation(const std::string& input, std::string& error);
    static std::string ToMuParserExpression(const std::string& expression);
};
