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
* @brief Enum of Gaussian color styles.
*/
enum GaussianColorStyles
{
    SummerDay,
    CoolBlue,
    HardRed,
    BlackAndWhite,
    Pastel,
    PsychExperience,
    VividColors,
    Custom
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
    Retro,
    Hakim,
    Aguamarina,
    CustomGradient
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