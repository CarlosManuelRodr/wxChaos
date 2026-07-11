#include "ScriptData.h"

ScriptData::ScriptData(ScriptDataType type)
{
    file = "";
    name = "";
    documentationPath = "";
    scriptCategory = ScriptCategory::Undefined;
    minX = maxX = minY = 0.0;
    defaultIter = 0;
    juliaVariety = false;
    redrawAlways = false;
    extColor = false;
    noSetMap = false;
    hasOrbit = false;
    isValid = type == ScriptDataType::Standard;
}
