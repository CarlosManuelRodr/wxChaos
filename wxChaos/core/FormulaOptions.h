#pragma once
#include <wx/string.h>
#include "types/FormulaType.h"

struct FormulaOptions
{
    wxString userFormula;
    FormulaType type;
    bool julia;
    int bailout;
};
