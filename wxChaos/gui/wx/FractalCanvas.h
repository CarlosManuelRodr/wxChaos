/**
* @file FractalCanvas.h
* @brief FractalCanvas related stuff.
*
* FractalCanvas is the class used to draw the fractal, the play button, the selection rect, and
* the screen pointer.
*
* @author Carlos Manuel Rodriguez Martinez
* @date 7/19/2012
*/

#pragma once

#include "wxSFMLCanvas.h"
#include "sfml/ToggleButton.h"
#include "sfml/SelectionRect.h"
#include "sfml/SFMLFractal.h"
#include "sfml/CoordinateSelector.h"
#include "../../core/FractalHandler.h"

wxDECLARE_EVENT(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, wxCommandEvent);

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
    SFMLFractal* _sfmlFractal;
    SelectionRect* _selection;
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

    FormulaOptions _userFormula;
    ScriptData _scriptData;

    sf::Event _event{};
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

    void OnUpdate() override;               ///< Handles the SFML events and the drawing of the textures.
    void CreateFractal(FractalType type);
    void CreateScriptFractal(const ScriptData& scriptData);

public:
    ///@brief Constructor
    ///@param fractalType Type of the fractal to be created.
    ///@param parent
    ///@param id
    ///@param position
    ///@param size
    ///@param style
    ///@param parent Parent wxWindow.
    FractalCanvas(FractalType fractalType, wxWindow* parent,
                  wxWindowID id, const wxPoint& position, const wxSize& size, long style = 0);
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
    SFMLFractal* GetSFMLFractalPtr() const;

    ///@return The type of the current fractal.
    FractalType GetFractalType() const;

    ///@return true if the current render can be aborted.
    bool CanAbortRender() const;

    ///@brief Aborts the active render, leaving the partial image visible.
    void AbortRender() const;

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

    ///@return true if orbit mode is active.
    bool IsOrbitMode() const;

    ///@brief Sets the slider mode.
    ///@param mode New mode.
    void SetSliderMode(bool mode);

    ///@return true if slider mode is active.
    bool IsSliderMode() const;

    ///@brief Sets the user formula.
    ///@param userFormula Formula specified by the user.
    void SetUserFormula(const FormulaOptions &userFormula);

    ///@return Formula in the user-defined fractal.
    FormulaOptions GetFormula();

    ///// Event processor /////

    ///@brief Updates the status bar of the MainFrame when the mouse is moved over the fractal canvas.
    void OnMoveMouse(wxMouseEvent& event);

    void OnClick(wxMouseEvent& event);
    void OnReleaseClick(wxMouseEvent& event);

    ///@brief Resizes the frame.
    void OnResize(wxSizeEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
};
