#pragma once
#include <wx/string.h>
#include "ColorPaletteTypes.h"

/**
* @class ColorPalette
* @brief Class that assign a wxString parameter according to the selected palette.
*/
class ColorPalette
{
public:
    int paletteSize;
    wxString grad;

    ///@brief Empty constructor.
    ColorPalette();

    ///@brief Assign wxString parameter according to the palette selected.
    ///@param palette Palette selected.
    void SetStyle(ColorPaletteTypes palette);
};
