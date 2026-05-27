#include <mpParser.h>
#include "FPUserDefinedRenderer.h"

FPUserDefinedRenderer::FPUserDefinedRenderer()
{
    _bailout = 0;
    _julia = false;
    _minStep = 0.001;
}

void FPUserDefinedRenderer::SetFormula(const FormulaOpt& formula)
{
    _bailout = formula.bailout;
    _julia = formula.julia;
    _parserFormula = formula.userFormula;
}

void FPUserDefinedRenderer::Render()
{
    mup::ParserX parser;
    parser.SetExpr(_parserFormula.wc_str());

    mup::Value zVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));

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
                for (n = 0; n < _maxIter; n++)
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
        _errorInfo = wxT("Error: ");
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

void FPUserDefinedRenderer::SetParams(const double minStep)
{
    _minStep = minStep;
}

wxString FPUserDefinedRenderer::GetErrorInfo()
{
    return _errorInfo;
}

void FPUserDefinedRenderer::ClearErrorInfo()
{
    _errorInfo.clear();
}

bool FPUserDefinedRenderer::IsThereError() const
{
    return !(_errorInfo.size() == 0);
}

