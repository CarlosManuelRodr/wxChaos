#include "UserFormulaSymbolicCompiler.h"

#include <algorithm>
#include <regex>
#include <symengine/basic.h>
#include <symengine/functions.h>
#include <symengine/parser/parser.h>
#include <symengine/simplify.h>
#include <symengine/symbol.h>
#include <symengine/symengine_casts.h>
#include "TextUtils.h"

bool UserFormulaSymbolicCompiler::ContainsUnsupportedTypeName(const std::string& typeName)
{
    static const std::string unsupported[] = {
        "ABS", "CONJUGATE", "DERIVATIVE", "SUBS", "FLOOR", "CEILING", "TRUNCATE", "SIGN",
        "LAMBERT", "ZETA", "DIRICHLET", "ATAN2", "GAMMA", "ERF"
    };

    std::string upper = typeName;
    std::transform(upper.begin(), upper.end(), upper.begin(), [](const unsigned char c)
    {
        return static_cast<char>(std::toupper(c));
    });

    return std::any_of(std::begin(unsupported), std::end(unsupported), [&upper](const std::string& value)
    {
        return upper.find(value) != std::string::npos;
    });
}

std::string UserFormulaSymbolicCompiler::GetUnsupportedFunctionName(const std::string& typeName)
{
    static const std::pair<std::string, std::string> unsupportedFunctions[] = {
        {"ABS", "abs"},
        {"CONJUGATE", "conjugate"},
        {"FLOOR", "floor"},
        {"CEILING", "ceiling"},
        {"TRUNCATE", "truncate"},
        {"SIGN", "sign"},
        {"LAMBERT", "lambert"},
        {"ZETA", "zeta"},
        {"DIRICHLET", "dirichlet"},
        {"ATAN2", "atan2"},
        {"GAMMA", "gamma"},
        {"ERF", "erf"}
    };

    std::string upper = typeName;
    std::transform(upper.begin(), upper.end(), upper.begin(), [](const unsigned char c)
    {
        return static_cast<char>(std::toupper(c));
    });

    const auto found = std::find_if(std::begin(unsupportedFunctions), std::end(unsupportedFunctions),
                                    [&upper](const auto& function)
    {
        return upper.find(function.first) != std::string::npos;
    });

    return found == std::end(unsupportedFunctions) ? std::string() : found->second;
}

bool UserFormulaSymbolicCompiler::ValidateExpressionTree(const SymEngine::RCP<const SymEngine::Basic>& expression, std::string& error)
{
    if (SymEngine::is_a<SymEngine::Symbol>(*expression))
    {
        const auto& symbol = SymEngine::down_cast<const SymEngine::Symbol&>(*expression);
        if (symbol.get_name() != "z")
        {
            error = "Unknown symbol: " + symbol.get_name() + ". Use z as the complex variable.";
            return false;
        }
    }

    if (SymEngine::is_a<SymEngine::FunctionSymbol>(*expression))
    {
        const auto& function = SymEngine::down_cast<const SymEngine::FunctionSymbol&>(*expression);
        error = "Unsupported function: " + function.get_name();
        return false;
    }

    const std::string typeName = SymEngine::type_code_name(expression->get_type_code());
    if (ContainsUnsupportedTypeName(typeName))
    {
        const std::string functionName = GetUnsupportedFunctionName(typeName);
        error = functionName.empty()
            ? "Unsupported symbolic construct: " + typeName
            : "Unsupported function: " + functionName;
        return false;
    }

    for (const auto& arg : expression->get_args())
    {
        if (!ValidateExpressionTree(arg, error))
            return false;
    }

    return true;
}

bool UserFormulaSymbolicCompiler::CompileNewtonFormula(const std::string& input, CompiledUserFormula& output, std::string& error)
{
    output = {};
    error.clear();

    const std::string normalized = NormalizeEquation(input, error);
    if (!error.empty())
        return false;

    try
    {
        SymEngine::Parser parser;
        const SymEngine::RCP<const SymEngine::Basic> expression = parser.parse(normalized);
        if (!ValidateExpressionTree(expression, error))
            return false;

        const SymEngine::RCP<const SymEngine::Symbol> z = SymEngine::symbol("z");
        const SymEngine::RCP<const SymEngine::Basic> derivative = SymEngine::expand(expression->diff(z));
        if (!ValidateExpressionTree(derivative, error))
            return false;

        output.expression = ToMuParserExpression(expression->__str__());
        output.derivativeExpression = ToMuParserExpression(derivative->__str__());
        return true;
    }
    catch (const std::exception& e)
    {
        error = "Parse error: ";
        error += e.what();
        return false;
    }
}

std::string UserFormulaSymbolicCompiler::NormalizeEquation(const std::string& input, std::string& error)
{
    error.clear();
    std::string trimmed = TextUtils::Trim(input);
    if (trimmed.empty())
    {
        error = "Parse error: formula is empty.";
        return {};
    }

    const size_t firstEquals = trimmed.find('=');
    if (firstEquals == std::string::npos)
        return trimmed;

    if (trimmed.find('=', firstEquals + 1) != std::string::npos)
    {
        error = "Parse error: use either f(z) or a single equation f(z) = 0.";
        return {};
    }

    const std::string left = TextUtils::Trim(trimmed.substr(0, firstEquals));
    const std::string right = TextUtils::Trim(trimmed.substr(firstEquals + 1));
    if (left.empty() || right.empty())
    {
        error = "Parse error: equation must have expressions on both sides of '='.";
        return {};
    }

    return "(" + left + ") - (" + right + ")";
}

std::string UserFormulaSymbolicCompiler::ToMuParserExpression(const std::string& expression)
{
    std::string converted = expression;

    size_t position = 0;
    while ((position = converted.find("**", position)) != std::string::npos)
    {
        converted.replace(position, 2, "^");
        position++;
    }

    converted = std::regex_replace(converted, std::regex("\\bE\\b"), "e");
    return converted;
}
