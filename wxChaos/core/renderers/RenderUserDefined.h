#pragma once
#include "../FractalClasses.h"

/*
* @class RenderUserDefined
* @brief Threaded UserDefined rendering routines.
*/
class RenderUserDefined : public RenderFractal
{
private:
    int bailout;
    bool julia;
    wxString errorInfo;
    wxString parserFormula;
public:
    RenderUserDefined();

    void SetFormula(FormulaOpt formula);
    void Render();
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError();
};
