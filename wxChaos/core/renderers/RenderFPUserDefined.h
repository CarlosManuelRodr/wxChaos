#pragma once
#include "FormulaOpt.h"
#include "../RenderFractal.h"

/*
* @class RenderFPUserDefined
* @brief Threaded FPUserDefined rendering routines.
*/
class RenderFPUserDefined : public RenderFractal
{
    int _bailout;
    bool _julia;
    double _minStep;
    wxString _errorInfo;
    wxString _parserFormula;
public:
    RenderFPUserDefined();

    void SetFormula(FormulaOpt formula);
    void Render() override;
    void SetParams(double minStep);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError();
};
