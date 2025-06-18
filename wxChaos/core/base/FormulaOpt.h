#pragma once
#include <wx/string.h>
#include "FormulaType.h"

struct FormulaOpt
{
    wxString userFormula;
    FormulaType type;
    bool julia;
    int bailout;
};
