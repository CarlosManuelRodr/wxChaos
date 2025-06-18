#pragma once

#include <string>
#include "ScriptCategory.h"
#include "ScriptDataType.h"

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

    ScriptData(ScriptDataType type = ScriptDataType::Standard);
};
