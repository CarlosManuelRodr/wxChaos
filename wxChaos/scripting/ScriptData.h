#pragma once

#include <string>
#include "types/ScriptCategory.h"
#include "types/ScriptDataType.h"

struct ScriptData
{
    std::string file;
    std::string name;
    std::string documentationPath;
    ScriptCategory scriptCategory;
    double minX, maxX, minY;
    int defaultIter;
    bool juliaVariety;
    bool redrawAlways;
    bool extColor;
    bool noSetMap;
    ///< Whether the compiled script declares a void DrawOrbit() entry point.
    bool hasOrbit;
    bool isValid;

    explicit ScriptData(ScriptDataType type = ScriptDataType::Standard);
};
