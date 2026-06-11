#pragma once
#include "FormulaOptions.h"
#include "../Renderer.h"

/*
* @class RenderFPUserDefined
* @brief Threaded FPUserDefined rendering routines.
*/
class UserDefinedFixedPointRenderer : public Renderer
{
    int _bailout;
    bool _julia;
    double _minStep;
    wxString _errorInfo;
    wxString _parserFormula;

public:
    UserDefinedFixedPointRenderer();

    void SetFormula(const FormulaOptions& formula);
    void Render() override;
    void SetParams(double minStep);
    void ClearErrorInfo();
    [[nodiscard]] wxString GetErrorInfo();
    [[nodiscard]] bool IsThereError() const;
};
