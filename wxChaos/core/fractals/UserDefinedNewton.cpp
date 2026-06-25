#include <algorithm>
#include <complex>
#include <cmath>
#include <mpParser.h>
#include "UserDefinedNewton.h"

bool UserDefinedNewton::IsFinite(const std::complex<double>& value)
{
    return std::isfinite(value.real()) && std::isfinite(value.imag());
}

UserDefinedNewton::UserDefinedNewton(const unsigned int width, const unsigned int height) : Fractal(width, height)
{
    _minX = -2.0;
    _maxX = 2.0;
    _minY = -1.5;
    _maxY = _minY + (_maxX - _minX) * _screenHeight / _screenWidth;

    _xFactor = (_maxX - _minX) / (_screenWidth - 1);
    _yFactor = (_maxY - _minY) / (_screenHeight - 1);

    _type = FractalType::NewtonUserDefined;
    _hasOrbit = true;
    _relativeColor = true;
    _redrawAlways = true;
    _renderJobCompatible = false;
    _threadNumber = 1;
    _myRender = new UserDefinedNewtonRenderer[_threadNumber];
    SetWatchdog<UserDefinedNewtonRenderer>(_myRender, &_watchdog, _threadNumber);

    _convergenceEpsilon = 1e-8;
    _functionEpsilon = 1e-8;
    _derivativeEpsilon = 1e-12;
    _rootTolerance = 1e-5;
    _escapeRadius = 1e6;

    _panelOpt.SetForceShow(true);
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Convergence epsilon: "), &_convergenceEpsilon, wxT("0.00000001"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Function epsilon: "), &_functionEpsilon, wxT("0.00000001"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Derivative epsilon: "), &_derivativeEpsilon, wxT("0.000000000001"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Root tolerance: "), &_rootTolerance, wxT("0.00001"));
    _panelOpt.LinkDbl(PanelOptionType::TextCtrl, wxT("Escape radius: "), &_escapeRadius, wxT("1000000"));

    _algorithm = RenderingAlgorithmType::ConvergenceTest;
    _availableAlg.push_back(RenderingAlgorithmType::ConvergenceTest);
}

UserDefinedNewton::~UserDefinedNewton()
{
    StopRender();
    delete[] _myRender;
}

bool UserDefinedNewton::HasCompiledFormula() const
{
    return !_compiledFormula.expression.empty() && !_compiledFormula.derivativeExpression.empty();
}

void UserDefinedNewton::ApplyRendererState() const
{
    for (unsigned int i = 0; i < _threadNumber; i++)
    {
        _myRender[i].SetParams(_convergenceEpsilon, _functionEpsilon, _derivativeEpsilon, _rootTolerance, _escapeRadius);
        if (HasCompiledFormula())
            _myRender[i].SetCompiledFormula(_compiledFormula.expression, _compiledFormula.derivativeExpression);
        else
            _myRender[i].SetFormulaError(_errorInfo);
    }
}

void UserDefinedNewton::Render()
{
    ApplyRendererState();
    SetRendererBounds<UserDefinedNewtonRenderer>(_myRender);
}

void UserDefinedNewton::SetFormula(const FormulaOptions formula)
{
    _userFormula = formula;
    if (std::string error; UserFormulaSymbolicCompiler::CompileNewtonFormula(formula.userFormula.utf8_string(), _compiledFormula, error))
        _errorInfo.clear();
    else
    {
        _compiledFormula = {};
        _errorInfo = wxString(error);
    }

    ApplyRendererState();
}

UserDefinedNewton::OrbitResult UserDefinedNewton::DrawNewtonOrbit()
{
    OrbitResult result;
    mup::ParserX functionParser;
    mup::ParserX derivativeParser;
    functionParser.SetExpr(_compiledFormula.expression);
    derivativeParser.SetExpr(_compiledFormula.derivativeExpression);

    mup::Value zVal;
    functionParser.DefineVar("z", mup::Variable(&zVal));
    functionParser.DefineVar("Z", mup::Variable(&zVal));
    derivativeParser.DefineVar("z", mup::Variable(&zVal));
    derivativeParser.DefineVar("Z", mup::Variable(&zVal));

    std::complex<double> z(_orbitX, _orbitY);
    for (unsigned int i = 0; i < _maxIter; i++)
    {
        const std::complex<double> previous = z;
        zVal = mup::cmplx_type(z.real(), z.imag());
        const mup::IValue& fValue = functionParser.Eval();
        const mup::IValue& dfValue = derivativeParser.Eval();
        const std::complex<double> fz(fValue.GetFloat(), fValue.GetImag());
        const std::complex<double> dfz(dfValue.GetFloat(), dfValue.GetImag());

        if (!IsFinite(fz) || !IsFinite(dfz) || std::abs(dfz) < _derivativeEpsilon)
        {
            result.failed = true;
            break;
        }

        const std::complex<double> step = fz / dfz;
        z = z - step;

        if (!IsFinite(step) || !IsFinite(z))
        {
            result.failed = true;
            break;
        }

        DrawLine(previous.real(), previous.imag(), z.real(), z.imag(), sf::Color(0, 255, 0), true);

        if (std::abs(step) < _convergenceEpsilon || std::abs(fz) < _functionEpsilon)
        {
            result.converged = true;
            break;
        }

        if (std::abs(z) > _escapeRadius)
        {
            result.failed = true;
            break;
        }
    }

    return result;
}

void UserDefinedNewton::DrawOrbit()
{
    if (HasCompiledFormula())
    {
        try
        {
            const OrbitResult result = DrawNewtonOrbit();
            if (result.failed && !_orbitLines.empty())
                _orbitLines.back().color = sf::Color(255, 0, 0);
        }
        catch (const mup::ParserError&)
        {
            if (!_orbitLines.empty())
                _orbitLines.back().color = sf::Color(255, 0, 0);
        }
    }

    _orbitDrawn = true;
}

void UserDefinedNewton::CopyOptFromPanel()
{
    _convergenceEpsilon = std::max(0.0, *_panelOpt.GetDoubleElement(0));
    _functionEpsilon = std::max(0.0, *_panelOpt.GetDoubleElement(1));
    _derivativeEpsilon = std::max(0.0, *_panelOpt.GetDoubleElement(2));
    _rootTolerance = std::max(0.0, *_panelOpt.GetDoubleElement(3));
    _escapeRadius = std::max(1.0, *_panelOpt.GetDoubleElement(4));
}

void UserDefinedNewton::PostRender()
{
    if (!_errorInfo.empty())
    {
        const wxString out = wxString(wxT("Fatal error in formula.\n")) + _errorInfo + wxT("\n");
        _errorInfo.clear();
        wxMessageDialog errorDialog(nullptr, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
        return;
    }

    if (_myRender[0].IsThereError())
    {
        const wxString out = wxString(wxT("Fatal error in formula.\n")) + _myRender[0].GetErrorInfo() + wxT("\n");
        _myRender[0].ClearErrorInfo();
        wxMessageDialog errorDialog(nullptr, out, wxT("Error"), wxOK | wxICON_ERROR);
        errorDialog.ShowModal();
    }
}
