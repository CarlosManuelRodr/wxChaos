#pragma once

#include <string>
#include "ColorPaletteTypes.h"

/**
* @class ColorPalette
* @brief Class that assigns gradient parameters according to the selected palette.
*/
class ColorPalette
{
public:
    int paletteSize;
    int colorCycleLength;
    std::string grad;

    ///@brief Empty constructor.
    ColorPalette();

    ///@brief Assign gradient parameters according to the palette selected.
    ///@param palette Palette selected.
    void SetStyle(ColorPaletteTypes palette);
};
