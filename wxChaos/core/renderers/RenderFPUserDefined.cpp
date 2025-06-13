#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"

RenderFPUserDefined::RenderFPUserDefined()
{
    bailout = 0;
    julia = false;
    minStep = 0.001;
}

void RenderFPUserDefined::SetFormula(FormulaOpt formula)
{
    bailout = formula.bailout;
    julia = formula.julia;
    parserFormula = formula.userFormula;
}

void RenderFPUserDefined::Render()
{
    mup::ParserX parser;
    parser.SetExpr(parserFormula.wc_str());

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
                    if ((z_prevVal.GetFloat() - minStep < zVal.GetFloat() &&
                         z_prevVal.GetFloat() + minStep > zVal.GetFloat()) &&
                        (z_prevVal.GetImag() - minStep < zVal.GetImag() &&
                         z_prevVal.GetImag() + minStep > zVal.GetImag()))
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
        errorInfo = wxT("Error: ");
        errorInfo += wxString(e.GetMsg());
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

void RenderFPUserDefined::SetParams(double _minStep)
{
    minStep = _minStep;
}

wxString RenderFPUserDefined::GetErrorInfo()
{
    return errorInfo;
}

void RenderFPUserDefined::ClearErrorInfo()
{
    errorInfo.clear();
}

bool RenderFPUserDefined::IsThereError()
{
    return !(errorInfo.size() == 0);
}

