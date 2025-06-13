#include "../FractalIncludes.h"
#include "../FractalTypes.h"
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
#include "../FractalUtils.h"
RenderUserDefined::RenderUserDefined()
{
    bailout = 1;
    julia = false;
}
void RenderUserDefined::SetFormula(FormulaOpt formula)
{
    bailout = formula.bailout;
    julia = formula.julia;
    parserFormula = formula.userFormula;
}
void RenderUserDefined::Render()
{
    mup::ParserX parser;
    parser.SetExpr(parserFormula.wc_str());

    unsigned n;
    int squaredBail = bailout*bailout;
    double z_y;

    // muParserX vars.
    mup::Value zVal;
    mup::Value cVal;
    mup::Value zero = mup::cmplx_type(0, 0);
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));
    if(julia) cVal = mup::cmplx_type(kReal, kImaginary);
    bool insideSet;

    // Parser execution.
    try
    {
        for(y=ho; y<hf; y++)
        {
            z_y = maxY - y*yFactor;
            for(x=wo; x<wf; x++)
            {
                if(!julia)
                {
                    cVal = mup::cmplx_type(minX + x*xFactor, z_y);
                    zVal = zero;
                }
                else
                    zVal = mup::cmplx_type(minX + x*xFactor, z_y);

                insideSet = true;
                for(n=0; n<maxIter; n++)
                {
                    if(zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                    zVal = parser.Eval();
                }
                if(insideSet)
                    setMap[x][y] = true;

                colorMap[x][y] = n;
            }
        }
    }
    catch(mup::ParserError& e)
    {
        // Reports error and fill screen with null values.
        errorInfo = wxT("Error: ");
        errorInfo += wxString(e.GetMsg());

        for(int y=ho; y<hf; y++)
        {
            for(int x=wo; x<wf; x++)
            {
                setMap[x][y] = false;
                colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}
wxString RenderUserDefined::GetErrorInfo()
{
    return errorInfo;
}
void RenderUserDefined::ClearErrorInfo()
{
    errorInfo.clear();
}
bool RenderUserDefined::IsThereError()
{
    return !(errorInfo.size() == 0);
}

