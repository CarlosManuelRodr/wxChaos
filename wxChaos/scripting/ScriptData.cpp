#include "ScriptData.h"

ScriptData::ScriptData(ScriptDataType type)
{
    file = "";
    name = "";
    scriptCategory = ScriptCategory::Undefined;
    minX = maxX = minY = 0.0;
    defaultIter = 0;
    juliaVariety = false;
    redrawAlways = false;
    extColor = false;
    noSetMap = false;

    if (type == ScriptDataType::Standard)
        isValid = true;
    else
        isValid = false;
}
