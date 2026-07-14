#include "UserDefinedFixedPointRenderer.h"
#include <mpParser.h>

UserDefinedFixedPointRenderer::UserDefinedFixedPointRenderer()
{
    _bailout = 0;
    _julia = false;
    _minStep = 0.001;
}

void UserDefinedFixedPointRenderer::SetFormula(const FormulaOptions& formula)
{
    _bailout = formula.bailout;
    _julia = formula.julia;
    _parserFormula = formula.userFormula;
}

void UserDefinedFixedPointRenderer::Render()
{
    mup::ParserX parser;
    parser.SetExpr(_parserFormula.utf8_string());

    mup::Value zVal;
    parser.DefineVar("z", mup::Variable(&zVal));
    parser.DefineVar("Z", mup::Variable(&zVal));

    try
    {
        double z_y;
        mup::Value z_prevVal;
        unsigned n;
        for (_y = _heightOrigin; _y < _heightFinal; _y++)
        {
            z_y = _maxY - _y * _yFactor;
            for (_x = _widthOrigin; _x < _widthFinal; _x++)
            {
                z_prevVal = zVal = mup::cmplx_type(_minX + _x * _xFactor, z_y);
                for (n = 0; n < _maxIterations; n++)
                {
                    zVal = parser.Eval();
                    if ((z_prevVal.GetFloat() - _minStep < zVal.GetFloat() &&
                         z_prevVal.GetFloat() + _minStep > zVal.GetFloat()) &&
                        (z_prevVal.GetImag() - _minStep < zVal.GetImag() &&
                         z_prevVal.GetImag() + _minStep > zVal.GetImag()))
                        break;
                    z_prevVal = zVal;
                }
                _colorMap[_x][_y] = (zVal.GetFloat() > 0 ? 1 : 30) + n;
            }
        }
    }
    catch (mup::ParserError &e)
    {
        _errorInfo = "Error: ";
        _errorInfo += wxString(e.GetMsg());
        for (int y = _heightOrigin; y < _heightFinal; y++)
        {
            for (int x = _widthOrigin; x < _widthFinal; x++)
            {
                _setMap[x][y] = false;
                _colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}

void UserDefinedFixedPointRenderer::SetParams(const double minStep)
{
    _minStep = minStep;
}

wxString UserDefinedFixedPointRenderer::GetErrorInfo()
{
    return _errorInfo;
}

void UserDefinedFixedPointRenderer::ClearErrorInfo()
{
    _errorInfo.clear();
}

bool UserDefinedFixedPointRenderer::IsThereError() const
{
    return _errorInfo.size() != 0;
}

