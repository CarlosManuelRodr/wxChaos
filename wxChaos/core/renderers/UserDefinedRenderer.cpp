#include <mpParser.h>
#include "UserDefinedRenderer.h"

UserDefinedRenderer::UserDefinedRenderer()
{
    bailout = 1;
    julia = false;
}
void UserDefinedRenderer::SetFormula(const FormulaOpt &formula)
{
    bailout = formula.bailout;
    julia = formula.julia;
    parserFormula = formula.userFormula;
}
void UserDefinedRenderer::Render()
{
    mup::ParserX parser;
    parser.SetExpr(parserFormula.wc_str());

    int squaredBail = bailout*bailout;

    // muParserX vars.
    mup::Value zVal;
    mup::Value cVal;
    mup::Value zero = mup::cmplx_type(0, 0);
    parser.DefineVar(_T("z"), mup::Variable(&zVal));
    parser.DefineVar(_T("c"),  mup::Variable(&cVal));
    parser.DefineVar(_T("Z"), mup::Variable(&zVal));
    parser.DefineVar(_T("C"),  mup::Variable(&cVal));

    if (julia)
        cVal = mup::cmplx_type(_kReal, _kImaginary);

    // Parser execution.
    try {
        bool insideSet;
        double z_y;
        unsigned n;
        for (_y=_heightOrigin; _y<_heightFinal; _y++)
        {
            z_y = _maxY - _y*_yFactor;
            for (_x=_widthOrigin; _x<_widthFinal; _x++)
            {
                if (!julia)
                {
                    cVal = mup::cmplx_type(_minX + _x*_xFactor, z_y);
                    zVal = zero;
                }
                else
                    zVal = mup::cmplx_type(_minX + _x*_xFactor, z_y);

                insideSet = true;
                for (n=0; n<_maxIter; n++)
                {
                    if (zVal.GetFloat()*zVal.GetFloat() + zVal.GetImag()*zVal.GetImag() > squaredBail)
                    {
                        insideSet = false;
                        break;
                    }
                    zVal = parser.Eval();
                }
                if (insideSet)
                    _setMap[_x][_y] = true;

                _colorMap[_x][_y] = n;
            }
        }
    }
    catch (mup::ParserError& e)
    {
        // Reports error and fill screen with null values.
        errorInfo = wxT("Error: ");
        errorInfo += wxString(e.GetMsg());

        for (int y=_heightOrigin; y<_heightFinal; y++)
        {
            for (int x=_widthOrigin; x<_widthFinal; x++)
            {
                _setMap[x][y] = false;
                _colorMap[x][y] = 0;
            }
        }
    }

    parser.ClearVar();
    parser.ClearFun();
}
wxString UserDefinedRenderer::GetErrorInfo()
{
    return errorInfo;
}
void UserDefinedRenderer::ClearErrorInfo()
{
    errorInfo.clear();
}
bool UserDefinedRenderer::IsThereError() const
{
    return errorInfo.size() != 0;
}

