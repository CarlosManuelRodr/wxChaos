#pragma once
#include "FormulaOpt.h"
#include "../RenderFractal.h"

/*
* @class RenderUserDefined
* @brief Threaded UserDefined rendering routines.
*/
class RenderUserDefined : public RenderFractal
{
    int bailout;
    bool julia;
    wxString errorInfo;
    wxString parserFormula;
public:
    RenderUserDefined();

    void SetFormula(const FormulaOpt &formula);
    void Render() override;
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError() const;
};
