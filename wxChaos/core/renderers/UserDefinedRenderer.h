#pragma once
#include "FormulaOpt.h"
#include "../Renderer.h"

/*
* @class RenderUserDefined
* @brief Threaded UserDefined rendering routines.
*/
class UserDefinedRenderer : public Renderer
{
    int bailout;
    bool julia;
    wxString errorInfo;
    wxString parserFormula;
public:
    UserDefinedRenderer();

    void SetFormula(const FormulaOpt &formula);
    void Render() override;
    wxString GetErrorInfo();
    void ClearErrorInfo();
    bool IsThereError() const;
};
