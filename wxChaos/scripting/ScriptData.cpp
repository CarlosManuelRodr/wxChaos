#include "ScriptData.h"

ScriptData::ScriptData(ScriptDataType type)
{
    file = "";
    name = "";
    documentationPath = "";
    horizontalCoordinate = "";
    verticalCoordinate = "";
    scriptCategory = ScriptCategory::Undefined;
    minX = maxX = minY = 0.0;
    defaultIter = 0;
    juliaVariety = false;
    redrawAlways = false;
    extColor = false;
    disableSetMap = false;
    hasOrbit = false;
    isValid = type == ScriptDataType::Standard;
}
