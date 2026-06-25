#pragma once
#include "FormulaOptions.h"
#include "../RenderWorker.h"

/**
* @class UserDefinedFixedPointRenderer
* @brief Renders user-defined fixed-point convergence formulas.
*
* The renderer evaluates the configured muParserX expression for each orbit
* step and records convergence state in the shared output maps.
*/
class UserDefinedFixedPointRenderer : public RenderWorker
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
