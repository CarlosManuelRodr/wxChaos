/** 
* @file FractalGUI.h 
* @brief This header file contains the GUI elements that are found in the FractalCanvas.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once
#ifndef _fractalGUI
#define _fractalGUI
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <wx/wx.h>
using namespace std;

/**
* @class Button
* @brief A simple SFML button.
*/
class Button
{
protected:
    sf::Image texture;            ///< Button texture.
    sf::Sprite output;            ///< Button sprite.
    sf::Font font;                ///< Font to draw some text into the button.
    sf::String buttonText;        ///< Text into the button.
    sf::Rect<double> area;        ///< Position of the button.
    bool pressed;                 ///< Status of the button.
    double screenWidth;
    double screenHeight;
    double FX, FY;
    bool anchorage;                ///< Status of anchor points.
    bool thereIsText;
    int anchorType;                ///< Position of anchor points.

    int buttonWidth;
    int buttonHeight;

public:
    ///@brief Calculates new position for the button, but stays the same size.
    ///@param Window Pointer to window that has been resized.
    void Resize(sf::RenderWindow* Window);

    ///@brief Constructor for a button that uses a image.
    ///@param Path Path to image file.
    ///@param posX Horizontal position of the button.
    ///@param posY Vertical position of the button.
    ///@param Window Window that will hold the button.
    Button(string Path, int posX, int posY, sf::RenderWindow* Window);

    ///@brief Constructor for a button that uses text.
    ///@param posX Horizontal position of the button.
    ///@param posY Vertical position of the button.
    ///@param Window Window that will hold the button.
    ///@param text Text of the button.
    Button(int posX, int posY, sf::RenderWindow* Window, string text);

    ///@brief Shows the button.
    ///@param Window Window to draw the button.
    void Show(sf::RenderWindow* Window);

    /**
    * @brief Sets anchorage position for the button.
    *
    * Currently there are only two position defined: Botton left and bottom right. This is
    * because there hasn't been a need to define more since the program depends more on wxWidgets
    * for rendering a GUI.
    * @param Top Top anchorage.
    * @param Left Left anchorage.
    * @param Bottom Bottom anchorage.
    * @param Right Right anchorage.
    */
    void SetAnchorage(bool Top, bool Left, bool Bottom, bool Right);

    ///@brief The user clicked the button and it changes it's colors.
    void ChangeState();

    ///@brief Handle a click event.
    ///@param Event A SFML event.
    virtual bool HandleEvents( sf::Event Event );
};


/**
* @class ButtonChange
* @brief A simple SFML that changes it's image when pressed.
*/
class ButtonChange : public Button
{
    sf::Image texture2;        ///< Second button texture.

public:
    ///@brief Constructor
    ///@param Path1 Filepath of the first image.
    ///@param Path2 Filepath of the second image.
    ///@param posX Horizontal position of the button.
    ///@param posY Vertical position of the button.
    ///@param Window Window that will hold the button.
    ButtonChange(string Path1, string Path2, int posX, int posY, sf::RenderWindow* Window);

    ///@brief Handle a click event.
    ///@param Event A SFML event.
    bool HandleEvents(sf::Event Event);

    bool ClickEvent(wxMouseEvent& event);

    ///@brief Resets button to the first image and unpressed value.
    void Reset();
};


/**
* @class SelectRect
* @brief A selection rectangle to make a zoom into the fractal.
*/
class SelectRect
{
    sf::Rect<int> pos;                    ///< Current selection rect.
    sf::Rect<int> select;                 ///< Final selection rect.
    sf::Image texture;                            
    sf::Sprite output;
    int x, y;
    int xSelect, ySelect;

    sf::Color color;
    bool inSelection;                    ///< Selection mode.

public:
    ///@brief Constructor
    ///@param Window Window that will hold the selection rect.
    SelectRect(sf::RenderWindow* Window);

    ///@brief Shows the selection rect.
    ///@param Window Window that will show the selection rect.
    void Show(sf::RenderWindow* Window);

    ///@brief Handle a click event.
    ///@param Event A SFML event.
    bool HandleEvents(sf::Event Event);

    // WX events.
    void ClickEvent(wxMouseEvent& event);
    bool UnClickEvent(wxMouseEvent& event);
    void MoveEvent(wxMouseEvent& event);

    ///@brief Gets selection rect.
    ///@return A rectangle with the selected area.
    sf::Rect<int> GetSeleccion();
};

/**
* @enum PanelOptionType
* @brief Type of option panel element.
*/

enum class PanelOptionType
{
    Label, TextCtrl, Spin, CheckBox
};

/**
* @enum LinkTo
* @brief Type of variable that the option panel element will be linked to.
*/
enum class LinkTo
{
    ToInt, ToDouble, ToBool
};


/**
* @class PanelOptions
* @brief Allows a fractal to set wxWidgets elements into the MainFrame and link them with a variable.
*
* When a new fractal is loaded the main panel will look for it's PanelOptions element to retrieve a list of widgets to
* create and the variables that are linked with them.
* Each fractal declares the elements of PanelOptions in the constructor along with the variables that will be linked.
*/
class PanelOptions
{
    vector<PanelOptionType> type;      ///< Type of widget to draw.
    vector<wxString> label;            ///< String elements of label widgets.
    vector<wxString> defaults;         ///< Default text values to start the widgets.
    vector<LinkTo> linkTo;             ///< Type of variable to link.
    vector<int*> intTarget;            ///< Int's adresses.
    vector<double*> dblTarget;         ///< Double's adresses.
    vector<bool*> boolTarget;          ///< Bool's adresses.
    bool forceShow;                    ///< The panel is automatically shown.

public:
    ///@brief Constructor.
    PanelOptions();

    ///@brief Sets a widget element that will be linked to a integer.
    ///@param pType Type of widget.
    ///@param labelText Text of the label.
    ///@param linkInt Adress of the integer that will be linked.
    ///@param Default value of the widget.
    void LinkInt(PanelOptionType pType, wxString labelTxt, int* linkInt, wxString defaultVal);

    ///@brief Sets a widget element that will be linked to a double.
    ///@param pType Type of widget.
    ///@param labelText Text of the label.
    ///@param linkDbl Adress of the double that will be linked.
    ///@param Default value of the widget.
    void LinkDbl(PanelOptionType pType, wxString labelTxt, double* linkDbl, wxString defaultVal);

    ///@brief Sets a widget element that will be linked to a bool.
    ///@param pType Type of widget.
    ///@param labelText Text of the label.
    ///@param linkBool Adress of the bool that will be linked.
    ///@param Default value of the widget.
    void LinkBool(PanelOptionType pType, wxString labelTxt, bool* linkBool, wxString defaultVal);

    ///@brief Gets the number of widgets declared.
    ///@return An integer with the number of widgets.
    int GetElementsSize();

    ///@brief Get the linking type at the specified index.
    ///@param index Index of widget to look.
    LinkTo GetLinkType(int index);

    ///@brief Get the label element at the specified index.
    ///@param index Index of widget to look.
    wxString GetLabelElement(int index);

    ///@brief Get the int element at the specified index.
    ///@param index Index of widget to look.
    int* GetIntElement(int index);

    ///@brief Get the double element at the specified index.
    ///@param index Index of widget to look.
    double* GetDoubleElement(int index);

    ///@brief Get the bool element at the specified index.
    ///@param index Index of widget to look.
    bool* GetBoolElement(int index);

    ///@brief Get the default label at the specified index.
    ///@param index Index of widget to look.
    wxString GetDefault(int index);

    ///@brief Get the widget type at the specified index.
    ///@param index Index of widget to look.
    PanelOptionType GetPanelOptType(int index);

    ///@brief Sets force show mode.
    ///@param mode New mode.
    void SetForceShow(bool mode);

    ///@brief Gets force show mode.
    ///@return true if force show is set. false if not.
    bool GetForceShow();
};

#endif