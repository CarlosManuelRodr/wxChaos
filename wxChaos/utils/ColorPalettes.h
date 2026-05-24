/** 
* @file ColorPalettes.h
* @brief Contains palette related enums and classes.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez Martinez
* @date 7/18/2012
*/

#ifndef _color_palettes
#define _color_palettes
#include <wx/string.h>

/**
* @enum ColorPalettes
* @brief Enum of color palettes.
*/
enum ColorPalettes
{
    Retro,
    Hakim,
    Aguamarina,
    PastelDream,
    RoseGold,
    Gunmetal,
    SunsetDrive,
    AuroraBorealis,
    Vaporwave,
    DeepOcean,
    Ember,
    RainbowFire,
    CustomGradient
};

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
    void SetStyle(ColorPalettes palette);
};

#endif
