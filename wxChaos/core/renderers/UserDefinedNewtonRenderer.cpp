#include "UserDefinedNewtonRenderer.h"

#include <algorithm>
#include <cmath>
#include <mpParser.h>

UserDefinedNewtonRenderer::UserDefinedNewtonRenderer()
{
    _convergenceEpsilon = 1e-8;
    _functionEpsilon = 1e-8;
    _derivativeEpsilon = 1e-12;
    _rootTolerance = 1e-5;
    _escapeRadius = 1e6;
}

void UserDefinedNewtonRenderer::SetCompiledFormula(const std::string& functionExpression, const std::string& derivativeExpression)
{
    _functionExpression = functionExpression;
    _derivativeExpression = derivativeExpression;
    _errorInfo.clear();
}

void UserDefinedNewtonRenderer::SetFormulaError(const wxString& errorInfo)
{
    _functionExpression.clear();
    _derivativeExpression.clear();
    _errorInfo = errorInfo;
}

void UserDefinedNewtonRenderer::SetParams(const double convergenceEpsilon, const double functionEpsilon,
                                          const double derivativeEpsilon, const double rootTolerance,
                                          const double escapeRadius)
{
    _convergenceEpsilon = convergenceEpsilon;
    _functionEpsilon = functionEpsilon;
    _derivativeEpsilon = derivativeEpsilon;
    _rootTolerance = rootTolerance;
    _escapeRadius = escapeRadius;
}

bool UserDefinedNewtonRenderer::HasCompiledFormula() const
{
    return !_functionExpression.empty() && !_derivativeExpression.empty();
}

bool UserDefinedNewtonRenderer::IsFinite(const std::complex<double>& value)
{
    return std::isfinite(value.real()) && std::isfinite(value.imag());
}

double UserDefinedNewtonRenderer::ColorValue(const unsigned int rootId, const IterationResult& result) const
{
    constexpr double rootStride = 37.0;
    const double maxIterations = std::max(1.0, _maxIter);
    const double iterationRatio = 1.0 - std::min(1.0, static_cast<double>(result.iterations) / maxIterations);
    const double residualRatio = 1.0 - std::min(1.0, result.finalResidual / std::max(_functionEpsilon, 1e-300));
    const double stepRatio = 1.0 - std::min(1.0, result.finalStep / std::max(_convergenceEpsilon, 1e-300));
    const double convergenceFraction = std::clamp((residualRatio + stepRatio) * 0.5, 0.0, 1.0);
    return rootId * rootStride + iterationRatio * (rootStride - 1.0) + convergenceFraction;
}

void UserDefinedNewtonRenderer::FillFailedRegion() const
{
    for (int y = _heightOrigin; y < _heightFinal; y++)
    {
        for (int x = _widthOrigin; x < _widthFinal; x++)
        {
            _setMap[x][y] = true;
            _colorMap[x][y] = InvalidColor;
        }
    }
}

void UserDefinedNewtonRenderer::Render()
{
    if (!HasCompiledFormula())
    {
        if (_errorInfo.empty())
            _errorInfo = wxT("Could not compile Newton formula.");
        FillFailedRegion();
        return;
    }

    _rootRegistry.Clear();
    _rootRegistry.SetTolerance(_rootTolerance);

    mup::ParserX functionParser;
    mup::ParserX derivativeParser;
    functionParser.SetExpr(_functionExpression);
    derivativeParser.SetExpr(_derivativeExpression);

    mup::Value zVal;
    functionParser.DefineVar("z", mup::Variable(&zVal));
    functionParser.DefineVar("Z", mup::Variable(&zVal));
    derivativeParser.DefineVar("z", mup::Variable(&zVal));
    derivativeParser.DefineVar("Z", mup::Variable(&zVal));

    try
    {
        RenderPixels([this, &functionParser, &derivativeParser, &zVal](const double pixelRe, const double pixelIm)
        {
            IterationResult result;
            std::complex<double> z(pixelRe, pixelIm);

            for (unsigned int i = 0; i < static_cast<unsigned int>(_maxIter); i++)
            {
                zVal = mup::cmplx_type(z.real(), z.imag());
                const mup::IValue& fValue = functionParser.Eval();
                const mup::IValue& dfValue = derivativeParser.Eval();
                const std::complex<double> fz(fValue.GetFloat(), fValue.GetImag());
                const std::complex<double> dfz(dfValue.GetFloat(), dfValue.GetImag());

                if (!IsFinite(fz) || !IsFinite(dfz))
                    break;

                const double derivativeMagnitude = std::abs(dfz);
                if (derivativeMagnitude < _derivativeEpsilon)
                    break;

                const std::complex<double> step = fz / dfz;
                const std::complex<double> zNext = z - step;

                if (!IsFinite(step) || !IsFinite(zNext))
                    break;

                const double stepMagnitude = std::abs(step);
                const double residualMagnitude = std::abs(fz);
                result.iterations = i + 1;
                result.finalZ = zNext;
                result.finalStep = stepMagnitude;
                result.finalResidual = residualMagnitude;

                if (stepMagnitude < _convergenceEpsilon || residualMagnitude < _functionEpsilon)
                {
                    result.converged = true;
                    break;
                }

                if (std::abs(zNext) > _escapeRadius)
                    break;

                z = zNext;
            }

            if (!result.converged)
            {
                _setMap[_x][_y] = true;
                _colorMap[_x][_y] = InvalidColor;
                return;
            }

            const unsigned int rootId = _rootRegistry.ClassifyOrAdd(result.finalZ);
            _colorMap[_x][_y] = ColorValue(rootId, result);
        });
    }
    catch (const mup::ParserError& e)
    {
        _errorInfo = wxT("Could not compile derivative expression.\n");
        _errorInfo += wxString(e.GetMsg());
        FillFailedRegion();
    }
}

wxString UserDefinedNewtonRenderer::GetErrorInfo() const
{
    return _errorInfo;
}

void UserDefinedNewtonRenderer::ClearErrorInfo()
{
    _errorInfo.clear();
}

bool UserDefinedNewtonRenderer::IsThereError() const
{
    return !_errorInfo.empty();
}
