/**
* @file FractalCanvas.h
* @brief FractalCanvas related stuff.
*
* FractalCanvas is the class used to draw the fractal, the play button, the selection rect and
* the screenpointer.
*
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once
#ifndef _fractalCanvas
#define _fractalCanvas

#include "wxSFMLCanvas.h"
#include "FractalTypes.h"

/**
* @struct PauseContinueButton
* @brief Holds a pointer to the pause menu item and it's state.
*/
struct PauseContinueButton
{
    wxMenuItem* pauseContinue;
    bool state;
};

/**
* @struct MainWindowStatus
* @brief Pointers to menu items.
*/
struct MainWindowStatus
{
    wxStatusBar* status;
    wxMenuItem* showOrbit;
    wxMenuItem* slider;
};


/**
* @class FractalCanvas
* @brief This is where the fractal is drawn.
*
* FractalCanvas handles all the memory related stuff that happens when a new fractal is selected or the program is opened or closed.
* It's main purpose is to control the flow of the SFML events and to draw the generated image along with some GUI elements.
*/
class FractalCanvas : public wxSFMLCanvas
{
    // Fractal
    FractalHandler fractalHandler;
    SelectRect* selection;
    ButtonChange* play;
    ScreenPointer* screenPointer;
    FractalType type;
    Fractal* target;                    ///< Internally the methods of this class communicate with the fractal through this pointer.

    sf::Image keybImage;                ///< Texture of the info images.
    sf::Image mouseImage;
    sf::Image helpImage;
    sf::Sprite outKeyb;                 ///< Sprite of the info images.
    sf::Sprite outMouse;
    sf::Sprite outHelp;

    /* Since Linux can't handle wxSfml and a separate Sfml window the Julia fractal will be
       displayed on the fractal canvas. */
#ifdef __linux__
    sf::Image juliaImage;
    sf::Sprite outJulia;
    FractalHandler juliaHandler;
#endif

    FormulaOpt userFormula;
    ScriptData scriptData;

    sf::Event event;
    wxSize wSize;                          ///< Size of the fractalCanvas.
    virtual void OnUpdate();               ///< Handles the SFML events and the drawing of the textures.

    bool juliaMode;                        ///< State of the Julia mode.
    double kReal;
    double kImaginary;
    double prevKReal;
    double prevKImag;
    bool pointerChange;                    ///< If there is a change in the pointer this one is activated.
    bool keybGuide;                        ///< State of the keyboard guide.
    bool keybGuideMode;
    bool helpImageMode;
    bool orbitMode;
    bool sliderMode;
    bool onUpdate;

    MainWindowStatus statusData;
    PauseContinueButton* btn;

public :
    ///@brief Constructor
    ///@param status Pointer to the status bar of the MainFrame.
    ///@param pcb Pointer to struct that holds the adress of the pause button and its status.
    ///@param fractType Type of the fractal to be created.
    ///@param parent Parent wxWindow.
    FractalCanvas(MainWindowStatus status, PauseContinueButton* pcb, FractalType fractType, wxWindow* Parent, wxWindowID Id,
                  const wxPoint& Position, const wxSize& Size, long Style = 0);
    ~FractalCanvas();

    ///@brief Changes the size of the canvas.
    ///@param size New size. Must be the size of the parent container.
    void SetWxSize(wxSize size);

    ///@brief Changes the Julia mode.
    ///@param mode New mode.
    void SetJuliaMode(bool mode);

    ///@return Real value of the K constant.
    double GetKReal();

    ///@return Imaginary value of the K constant.
    double GetKImaginary();

    ///@brief State of the ScreenPointer.
    ///@return true if there was a change in the pointer. false if not.
    bool ChangeInPointer();

    ///@brief Changes the fractal type.
    ///@param _type New fractal type.
    void ChangeType(FractalType _type);

    ///@brief Like ChangeType but used when a script fractal selected.
    ///@param _scriptData Script parameters.
    void ChangeToScript(ScriptData _scriptData);

    ///@return A pointer to the fractal.
    Fractal* GetFractalPtr();

    ///@brief Sets the keyboard guide mode.
    ///@param mode New mode.
    void SetKeybGuide(bool mode);

    ///@brief Shows the help image.
    void ShowHelpImage();

    ///@brief Resets the fractal. Internally it deletes it and creates a new one exactly the same.
    void Reset();

    ///@brief Sets the orbit mode.
    ///@param mode New mode.
    void SetOrbitMode(bool mode);

    ///@brief Sets the slider mode.
    ///@param mode New mode.
    void SetSliderMode(bool mode);

    ///@brief Sets the user formula.
    ///@param _userFormula Formula specified by the user.
    void SetUserFormula(FormulaOpt _userFormula);

    ///@return Formula in the user defined fractal.
    FormulaOpt GetFormula();

    ///@brief Updates status bar of the MainFrame when the mouse is moved over the fractal canvas.
    void OnMoveMouse(wxMouseEvent& event);

    void OnClick(wxMouseEvent& event);
    void OnUnClick(wxMouseEvent& event);

    ///@brief Resizes the frame.
    void OnResize(wxSizeEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
};

extern FractalCanvas* fractalCanvasPtr;

#endif //__fCanvas