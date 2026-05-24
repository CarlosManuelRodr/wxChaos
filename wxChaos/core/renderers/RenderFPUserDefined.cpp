#include <mpParser.h>
#include "RenderFPUserDefined.h"

RenderFPUserDefined::RenderFPUserDefined()
{
    _bailout = 0;
    _julia = false;
    _minStep = 0.001;
}

void RenderFPUserDefined::SetFormula(FormulaOpt formula)
{
    _bailout = formula.bailout;
    _julia = formula.julia;
    _parserFormula = formula.userFormula;
}

void RenderFPUserDefined::Render()
{
    mup::ParserX parser;
    parser.SetExpr(_parserFormula.wc_str());

    double z_y;
    mup::Value zVal, z_prevVal;
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));

    try
    {
        unsigned n;
        for (y = ho; y < hf; y++)
        {
            z_y = maxY - y * yFactor;
            for (x = wo; x < wf; x++)
            {
                z_prevVal = zVal = mup::cmplx_type(minX + x * xFactor, z_y);
                for (n = 0; n < maxIter; n++)
                {
                    zVal = parser.Eval();
                    if ((z_prevVal.GetFloat() - _minStep < zVal.GetFloat() &&
                         z_prevVal.GetFloat() + _minStep > zVal.GetFloat()) &&
                        (z_prevVal.GetImag() - _minStep < zVal.GetImag() &&
                         z_prevVal.GetImag() + _minStep > zVal.GetImag()))
                        break;
                    else
                        z_prevVal = zVal;
                }
                if (zVal.GetFloat() > 0)
                    colorMap[x][y] = 1 + n;
                else
                    colorMap[x][y] = 30 + n;
            }
        }
    }
    catch (mup::ParserError &e)
    {
        _errorInfo = wxT("Error: ");
        _errorInfo += wxString(e.GetMsg());
        for (int y = ho; y < hf; y++)
        {
            for (int x = wo; x < wf; x++)
            {
                setMap[x][y] = false;
                colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}

void RenderFPUserDefined::SetParams(double minStep)
{
    _minStep = minStep;
}

wxString RenderFPUserDefined::GetErrorInfo()
{
    return _errorInfo;
}

void RenderFPUserDefined::ClearErrorInfo()
{
    _errorInfo.clear();
}

bool RenderFPUserDefined::IsThereError()
{
    return !(_errorInfo.size() == 0);
}

