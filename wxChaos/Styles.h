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
* @enum GAUSS_STYLES
* @brief Enum of EST styles.
*/
enum GAUSS_STYLES
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
    int redInt , greenInt, blueInt;
    double redMed, greenMed, blueMed;
    double redDes, greenDes, blueDes;
    int paletteSize;

    ///@brief Empty constructor.
    GaussianColorPalette();

    ///@brief Assigns parameters according to the style selected.
    ///@param style Style selected.
    void SetStyle(GAUSS_STYLES style);
};

/**
* @enum GRAD_STYLES
* @brief Enum of gradient styles.
*/
enum GRAD_STYLES
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
    void SetStyle(GRAD_STYLES style);
};

#endif