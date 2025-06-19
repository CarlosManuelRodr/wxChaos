#pragma once

#include <string>
#include "types/ScriptCategory.h"
#include "types/ScriptDataType.h"

struct ScriptData
{
    std::string file;
    std::string name;
    ScriptCategory scriptCategory;
    double minX, maxX, minY;
    int defaultIter;
    bool juliaVariety;
    bool redrawAlways;
    bool extColor;
    bool noSetMap;
    bool isValid;

    explicit ScriptData(ScriptDataType type = ScriptDataType::Standard);
};
