#pragma once
#include "FormulaOptions.h"
#include "../Renderer.h"

/*
* @class RenderUserDefined
* @brief Threaded UserDefined rendering routines.
*/
class UserDefinedEscapeTimeRenderer : public Renderer
{
    int bailout;
    bool julia;
    wxString errorInfo;
    wxString parserFormula;

public:
    UserDefinedEscapeTimeRenderer();

    void SetFormula(const FormulaOptions &formula);
    void Render() override;
    void ClearErrorInfo();
    [[nodiscard]] wxString GetErrorInfo();
    [[nodiscard]] bool IsThereError() const;
};
