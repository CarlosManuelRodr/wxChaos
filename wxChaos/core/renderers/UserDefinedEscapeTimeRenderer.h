#pragma once
#include "FormulaOptions.h"
#include "../RasterRenderWorker.h"

/**
* @class UserDefinedEscapeTimeRenderer
* @brief Renders user-defined escape-time formulas for one pixel region.
*
* The renderer evaluates the configured muParserX expression for each orbit
* step and writes escape state into the shared output maps.
*/
class UserDefinedEscapeTimeRenderer : public RasterRenderWorker
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
