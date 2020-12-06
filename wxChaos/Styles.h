/** 
* @file Styles.h 
* @brief Contains style related enums and classes.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/18/2012
*/

#ifndef _styles
#define _styles
#include <wx/string.h>

/**
* @enum GaussianColorStyles
* @brief Enum of EST styles.
*/
enum GaussianColorStyles
{
    SUMMER_DAY,
    COOL_BLUE,
    HARD_RED,
    BLACK_AND_WHITE,
    PASTEL,
    PSYCH_EXPERIENCE,
    VIVID_COLORS,
    CUSTOM
};


/**
* @class GaussianColorPalette
* @brief Class that assigns parameters according to the selected style.
*/
class GaussianColorPalette
{
public:
    int redInt, greenInt, blueInt;
    double redMean, greenMean, blueMean;
    double redStdDev, greenStdDev, blueStdDev;
    int paletteSize;

    ///@brief Empty constructor.
    GaussianColorPalette();

    ///@brief Assigns parameters according to the style selected.
    ///@param style Style selected.
    void SetStyle(GaussianColorStyles style);
};

/**
* @enum GradientColorStyles
* @brief Enum of gradient styles.
*/
enum GradientColorStyles
{
    RETRO,
    HAKIM,
    AGUAMARINA,
    GRAD_CUSTOM
};

/**
* @class GradientColorPalette
* @brief Class that assign a wxString parameter according to the selected style.
*/
class GradientColorPalette
{
public:
    int paletteSize;
    wxString grad;

    ///@brief Empty constructor.
    GradientColorPalette();

    ///@brief Assign wxString parameter according to the style selected.
    ///@param style Style selected.
    void SetStyle(GradientColorStyles style);
};

#endif