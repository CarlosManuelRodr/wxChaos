#pragma once

#include <string>
#include "types/ScriptCategory.h"
#include "types/ScriptDataType.h"
#include "ScriptOptions.h"

struct ScriptData
{
    std::string file;
    std::string name;
    std::string documentationPath;
    std::string horizontalCoordinate;
    std::string verticalCoordinate;
    ScriptCategory scriptCategory;
    double minX, maxX, minY;
    int defaultIter;
    bool juliaVariety;
    bool redrawAlways;
    bool extColor;
    bool disableSetMap;
    ///< Whether the compiled script declares a void DrawOrbit() entry point.
    bool hasOrbit;
    bool isValid;
    ScriptOptions options; ///< Arbitrary typed options declared by Configure().

    explicit ScriptData(ScriptDataType type = ScriptDataType::Standard);
};
