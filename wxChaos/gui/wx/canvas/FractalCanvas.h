/**
* @file FractalCanvas.h
* @brief wx/SFML canvas that hosts the interactive fractal view.
*
* FractalCanvas owns the wx event handlers and SFML overlay controls that let
* the user render, pan, zoom, select constants, inspect coordinates, and switch
* between concrete fractal models.
*
* @author Carlos Manuel Rodriguez Martinez
* @date 7/19/2012
*/

#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "canvas/wxSFMLCanvas.h"
#include "canvas/FractalInteractionTool.h"
#include "canvas/FractalTutorialModel.h"
#include "numeric/HighPrecision.h"
#include "FractalFactory.h"
#include "FormulaOptions.h"
#include "ScriptData.h"
#include "types/FractalType.h"

class wxPopupWindow;
class wxStaticText;
class Fractal;
class FractalPresenter;
class SelectionRect;
class CoordinateSelector;

wxDECLARE_EVENT(wxEVT_FRACTAL_CANVAS_STATUS_TEXT, wxCommandEvent);

/**
* @class FractalCanvas
* @brief Interactive wxWidgets/SFML surface for exploring the active fractal.
*
* FractalCanvas creates and swaps fractal models through FractalFactory, keeps
* the FractalPresenter attached to the current model, and coordinates the
* canvas-level controls: selection zoom, mouse-wheel zoom, mouse panning,
* Julia/orbit/slider coordinate selection, keyboard help overlays, render
* status text, and active interaction mode.
*/
class FractalCanvas : public wxSFMLCanvas
{
    FractalFactory _fractalFactory;
    FractalPresenter* _fractalPresenter;
    SelectionRect* _selectionRect;
    CoordinateSelector* _coordinateSelector;
    Fractal* _fractal;

    sf::Image _keyboardImage;               ///< Texture of the info images.
    sf::Image _mouseImage;
    sf::Image _clickDragSelectionImage;
    sf::Texture _keyboardTexture;
    sf::Texture _mouseTexture;
    sf::Texture _helpTexture;
    sf::Sprite _outKeyboard;                ///< Sprite of the info images.
    sf::Sprite _outMouse;
    sf::Sprite _outHelp;

    FormulaOptions _userFormula;
    ScriptData _scriptData;

    sf::Event _event{};
    sf::Clock _movementClock;
    wxSize _canvasSize;                     ///< Size of the fractalCanvas.

    bool _juliaMode;                        ///< State of the Julia mode.
    double _kReal;
    double _kImaginary;
    double _prevKReal;
    double _prevKImag;
    bool _coordinateSelectorChange;         ///< If there is a change in the pointer, this boolean is activated.
    bool _keyboardGuide;                    ///< State of the keyboard guide.
    bool _keyboardGuideMode;
    bool _guideImagesMode;
    bool _orbitMode;
    bool _sliderMode;
    bool _onUpdate;
    bool _hasLastMousePosition;
    bool _mouseWheelPanning;
    bool _toolPanning;
    bool _mousePanMoved;
    bool _zoomToolDragging;
    bool _showMainCanvasOverlays;
    wxPoint _lastMousePosition;
    wxPoint _lastMouseWheelPanPosition;
    wxPoint _zoomToolStartPosition;
    wxPopupWindow* _pointInfoPopup{};
    wxStaticText* _pointInfoText{};
    wxString _lastPointInfoText;

    void OnUpdate() override;               ///< Handles the SFML events and the drawing of the textures.
    void CreateFractal(FractalType type);
    void CreateScriptFractal(const ScriptData& scriptData);
    void AttachFractalStatusHandler();
    wxMouseEvent ToRenderMouseEvent(const wxMouseEvent& event) const;
    wxString BuildStatusText() const;
    void EmitStatusText();
    sf::Vector2u GetCurrentRenderSize() const;
    void ResizePresentation(wxSize size);
    void UpdateSelectionAspectRatio();
    void UpdateCoordinateSelectorValue();
    void SeedCoordinateSelectorValue();
    void BeginMousePanAt(wxPoint position);
    void ContinueMousePanAt(wxPoint position);
    void EndMousePanGesture();
    double CalculateZoomToolScale(wxPoint position) const;
    void CommitZoomToolDrag(wxPoint endPosition);
    wxString InspectPointAt(wxPoint position) const;
    void ShowPointInfo(wxPoint position, const wxString& text);
    void HidePointInfo();
    void CancelToolGestures();
    float CalculateKeyboardGuideScale() const;
    void UpdateKeyboardGuideImageLayout();
    void UpdateClickDragSelectionImageLayout();
    void UpdateOverlayLayout();
    unsigned int GetStatusCoordinateSignificantDigits() const;
    wxString FormatStatusCoordinate(const HighPrecisionReal& value) const;

    FractalInteractionTool _interactionTool;
    std::function<void(FractalTutorialAction)> _interactionCompletedHandler;

    void NotifyInteractionCompleted(FractalTutorialAction action) const;

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

    ///@brief Stops active rendering and idle refresh before the owning frame closes.
    void PrepareForClose();

    ///@brief Changes the Julia mode.
    ///@param mode New mode.
    void SetJuliaMode(bool mode);

    ///@return Real value of the K constant.
    double GetKReal() const;

    ///@return Imaginary value of the K constant.
    double GetKImaginary() const;

    ///@brief State of the ScreenPointer.
    ///@return true if there was a change in the pointer. false if not.
    bool ChangeInCoordinateSelector();

    ///@brief Changes the fractal type.
    ///@param type New fractal type.
    void ChangeType(FractalType type);

    ///@brief Like ChangeType but used when a script fractal selected.
    ///@param scriptData Script parameters.
    void ChangeToScript(const ScriptData &scriptData);

    ///@return A pointer to the fractal.
    Fractal* GetFractal() const;

    ///@return Current render size used by the embedded SFML canvas.
    sf::Vector2u GetRenderSize() const;

    ///@return A pointer to the SFML fractal presenter.
    FractalPresenter* GetFractalPresenter() const;

    ///@brief Changes the active mouse interaction tool.
    void SetInteractionTool(FractalInteractionTool tool);

    ///@return The active mouse interaction tool.
    FractalInteractionTool GetInteractionTool() const;

    ///@brief Installs a callback for semantic interactions completed on the canvas.
    void SetInteractionCompletedHandler(std::function<void(FractalTutorialAction)> handler);

    ///@return The type of the current fractal.
    FractalType GetFractalType() const;

    ///@return true if the current render can be aborted.
    bool CanAbortRender() const;

    ///@brief Aborts the active render, leaving the partial image visible.
    void AbortRender();

    ///@brief Sets the keyboard guide mode.
    ///@param mode New mode.
    void SetKeyboardGuide(bool mode);

    ///@brief Shows the help image.
    void ShowGuideImages();

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

    /// @brief Enables or disables overlays that belong only to the main window.
    /// @param show true to show iteration/status overlays, false for embedded previews.
    void SetMainCanvasOverlaysVisible(bool show);

    ///// Event processor /////

    ///@brief Updates the status bar of the MainFrame when the mouse is moved over the fractal canvas.
    void OnMoveMouse(wxMouseEvent& event);

    ///@brief Clears transient hover state when the mouse leaves the fractal canvas.
    void OnLeaveMouse(wxMouseEvent& event);

    void OnClick(wxMouseEvent& event);
    void OnReleaseClick(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

    ///@brief Resizes the frame.
    void OnResize(wxSizeEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
};
