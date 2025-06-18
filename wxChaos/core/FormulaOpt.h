#pragma once
#include <wx/string.h>
#include "types/FormulaType.h"

struct FormulaOpt
{
    wxString userFormula;
    FormulaType type;
    bool julia;
    int bailout;
};
