#pragma once
#include "../FractalClasses.h"
#include "../scripting/AngelscriptEngine.h"
* @class RenderFPUserDefined
* @brief Threaded FPUserDefined rendering routines.
*/
class RenderFPUserDefined : public RenderFractal
{
private:
    int bailout;
    bool julia;
    double minStep;
    wxString errorInfo;
    wxString parserFormula;
public:
    RenderFPUserDefined();

    void SetFormula(FormulaOpt formula);
    void Render();
    void SetParams(double _minStep);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError();
};
