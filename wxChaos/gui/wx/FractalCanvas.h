/**
* @file FractalCanvas.h
* @brief FractalCanvas related stuff.
*
* FractalCanvas is the class used to draw the fractal, the play button, the selection rect, and
* the screen pointer.
*
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once
#ifndef _fractalCanvas
#define _fractalCanvas

#include "sfml/ToggleButton.h"
#include "sfml/SelectRect.h"
#include "wxSFMLCanvas.h"
#include "../sfml/SFMLFractal.h"
#include "../sfml/CoordinateSelector.h"
#include "../../core/FractalHandler.h"

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
* FractalCanvas handles all the memory-related stuff that happens when a new fractal is selected or the program is opened or closed.
* Its main purpose is to control the flow of the SFML events and to draw the generated image along with some GUI elements.
*/
class FractalCanvas : public wxSFMLCanvas
{
    // Fractal
    FractalHandler _fractalHandler;
    SFMLFractal _sfmlFractal;
    SelectRect* _selection;
    ToggleButton* _play;
    CoordinateSelector* _screenPointer;
    FractalType _type;
    Fractal* _target;                       ///< Internally, the methods of this class communicate with the fractal through this pointer.

    sf::Image _keyboardImage;               ///< Texture of the info images.
    sf::Image _mouseImage;
    sf::Image _helpImage;
    sf::Texture _keyboardTexture;
    sf::Texture _mouseTexture;
    sf::Texture _helpTexture;
    sf::Sprite _outKeyboard;                ///< Sprite of the info images.
    sf::Sprite _outMouse;
    sf::Sprite _outHelp;

    FormulaOpt _userFormula;
    ScriptData _scriptData;

    sf::Event _event;
    wxSize _canvasSize;                     ///< Size of the fractalCanvas.

    bool _juliaMode;                        ///< State of the Julia mode.
    double _kReal;
    double _kImaginary;
    double _prevKReal;
    double _prevKImag;
    bool _pointerChange;                    ///< If there is a change in the pointer, this boolean is activated.
    bool _keyboardGuide;                    ///< State of the keyboard guide.
    bool _keyboardGuideMode;
    bool _helpImageMode;
    bool _orbitMode;
    bool _sliderMode;
    bool _onUpdate;

    MainWindowStatus statusData;
    PauseContinueButton* btn;

protected:
    void OnUpdate() override;               ///< Handles the SFML events and the drawing of the textures.

public:
    ///@brief Constructor
    ///@param status Pointer to the status bar of the MainFrame.
    ///@param pcb Pointer to struct that holds the adress of the pause button and its status.
    ///@param fractalType Type of the fractal to be created.
    ///@param parent
    ///@param id
    ///@param position
    ///@param size
    ///@param style
    ///@param parent Parent wxWindow.
    FractalCanvas(const MainWindowStatus &status, PauseContinueButton* pcb, FractalType fractalType, wxWindow* parent, wxWindowID id,
                  const wxPoint& position, const wxSize& size, long style = 0);
    ~FractalCanvas() override;

    ///@brief Changes the size of the canvas.
    ///@param size New size. Must be the size of the parent container.
    void SetWxSize(wxSize size);

    ///@brief Changes the Julia mode.
    ///@param mode New mode.
    void SetJuliaMode(bool mode);

    ///@return Real value of the K constant.
    double GetKReal() const;

    ///@return Imaginary value of the K constant.
    double GetKImaginary() const;

    ///@brief State of the ScreenPointer.
    ///@return true if there was a change in the pointer. false if not.
    bool ChangeInPointer();

    ///@brief Changes the fractal type.
    ///@param type New fractal type.
    void ChangeType(FractalType type);

    ///@brief Like ChangeType but used when a script fractal selected.
    ///@param scriptData Script parameters.
    void ChangeToScript(const ScriptData &scriptData);

    ///@return A pointer to the fractal.
    Fractal* GetFractalPtr() const;

    ///@return A pointer to the SFML fractal presenter.
    SFMLFractal* GetSFMLFractalPtr();

    ///@return The type of the current fractal.
    FractalType GetFractalType() const;

    ///@brief Sets the keyboard guide mode.
    ///@param mode New mode.
    void SetKeyboardGuide(bool mode);

    ///@brief Shows the help image.
    void ShowHelpImage();

    ///@brief Resets the fractal. Internally, it deletes it and creates a new one exactly the same.
    void Reset();

    ///@brief Sets the orbit mode.
    ///@param mode New mode.
    void SetOrbitMode(bool mode);

    ///@brief Sets the slider mode.
    ///@param mode New mode.
    void SetSliderMode(bool mode);

    ///@brief Sets the user formula.
    ///@param userFormula Formula specified by the user.
    void SetUserFormula(const FormulaOpt &userFormula);

    ///@return Formula in the user-defined fractal.
    FormulaOpt GetFormula();

    ///// Event processor /////

    ///@brief Updates the status bar of the MainFrame when the mouse is moved over the fractal canvas.
    void OnMoveMouse(wxMouseEvent& event);

    void OnClick(wxMouseEvent& event);
    void OnUnClick(wxMouseEvent& event);

    ///@brief Resizes the frame.
    void OnResize(wxSizeEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
};

extern FractalCanvas* fractalCanvasPtr;

#endif
