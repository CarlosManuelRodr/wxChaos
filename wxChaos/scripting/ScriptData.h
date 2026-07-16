#pragma once

#include <optional>
#include <string>
#include "types/ScriptCategory.h"
#include "types/ScriptDataType.h"
#include "ScriptOptions.h"

/** @brief Known-good inputs used to initialize a box-counting dimension calculation. */
struct DimensionCalculatorPreset
{
    double minX{};                     ///< Left edge of the square measurement region.
    double maxX{};                     ///< Right edge of the square measurement region.
    double minY{};                     ///< Bottom edge of the square measurement region.
    unsigned int iterations{};         ///< Render iteration limit.
    std::string divisionFunction;      ///< muParserX expression used to generate grid divisions.
    int functionXMin{};                ///< First input evaluated by the division function.
    int functionXMax{};                ///< Last input evaluated by the division function.
    int imageSize{};                   ///< Width and height of the full calculation image.
};

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
    bool dimensionCalculatorEnabled; ///< Whether the script explicitly opts into box-counting analysis.
    std::optional<DimensionCalculatorPreset> dimensionCalculatorPreset; ///< Known-good calculator inputs.
    ///< Whether the compiled script declares a void DrawOrbit() entry point.
    bool hasOrbit;
    bool isValid;
    ScriptOptions options; ///< Arbitrary typed options declared by Configure().

    explicit ScriptData(ScriptDataType type = ScriptDataType::Standard);
};
