#pragma once
#include "FormulaOpt.h"
#include "../Renderer.h"

/*
* @class RenderFPUserDefined
* @brief Threaded FPUserDefined rendering routines.
*/
class FPUserDefinedRenderer : public Renderer
{
    int _bailout;
    bool _julia;
    double _minStep;
    wxString _errorInfo;
    wxString _parserFormula;

public:
    FPUserDefinedRenderer();

    void SetFormula(const FormulaOpt& formula);
    void Render() override;
    void SetParams(double minStep);
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError() const;
};
