#include <doctest/doctest.h>
#include <mpParser.h>

#include <atomic>
#include <thread>
#include <vector>

#include "FormulaOptions.h"
#include "UserFormulaParser.h"
#include "fractals/raster/UserDefinedEscapeTime.h"
#include "fractals/raster/UserDefinedFixedPoint.h"

TEST_CASE("muParserX initialization is safe from concurrent callers")
{
    std::vector<std::thread> threads;
    threads.reserve(16);
    for (int i = 0; i < 16; i++)
        threads.emplace_back(UserFormulaParser::EnsureInitialized);

    for (auto& thread : threads)
        thread.join();

    CHECK(true);
}

TEST_CASE("muParserX debug token tracking is safe across parser threads")
{
    UserFormulaParser::EnsureInitialized();

    std::atomic<bool> evaluationsSucceeded = true;
    std::vector<std::thread> threads;
    threads.reserve(16);
    for (int thread = 0; thread < 16; thread++)
    {
        threads.emplace_back([&evaluationsSucceeded]
        {
            try
            {
                for (int evaluation = 0; evaluation < 25; evaluation++)
                {
                    mup::ParserX parser;
                    mup::Value zValue(mup::cmplx_type(0.25, 0.5));
                    parser.DefineVar("z", mup::Variable(&zValue));
                    parser.SetExpr("z = z^2 + 1");
                    parser.Eval();
                }
            }
            catch (...)
            {
                evaluationsSucceeded = false;
            }
        });
    }

    for (auto& thread : threads)
        thread.join();

    CHECK(evaluationsSucceeded);
}

TEST_CASE("user-defined formula fractals render after parser initialization")
{
    FormulaOptions escapeFormula;
    escapeFormula.type = FormulaType::Complex;
    escapeFormula.userFormula = "z = z^2 + c";
    escapeFormula.bailout = 2;

    UserDefinedEscapeTime escapeTime(32, 32);
    escapeTime.SetFormula(escapeFormula);
    escapeTime.RenderBlocking();
    CHECK_FALSE(escapeTime.IsRendering());

    FormulaOptions fixedPointFormula;
    fixedPointFormula.type = FormulaType::FixedPoint;
    fixedPointFormula.userFormula = "z = sin(z)";

    UserDefinedFixedPoint fixedPoint(32, 32);
    fixedPoint.SetFormula(fixedPointFormula);
    fixedPoint.RenderBlocking();
    CHECK_FALSE(fixedPoint.IsRendering());
}
