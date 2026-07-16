/**
* @file RendererOptionsFrame.h
* @brief This header contains the renderer options frame.
*
* @author Carlos Manuel Rodriguez Martinez
*
* @date 7/19/2012
*/

#pragma once

#include <functional>
#include <vector>
#include <wx/wx.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include <SFML/Graphics/Color.hpp>
#include "coloring/ColorPalette.h"
#include "Options.h"
#include "types/RenderingAlgorithmType.h"
#include "types/RenderingPrecisionMode.h"

class FractalPresenter;
class Fractal;

#define RendererOptionsFrameSize wxSize(1000, 1020)

wxDECLARE_EVENT(wxEVT_RENDERER_OPTIONS_CLOSED, wxCommandEvent);

/**
* @class RendererOptionsFrame
* @brief Frame that allows the user to change several color and algorithm parameters.
*/
class RendererOptionsFrame : public wxFrame
{
    wxScrolledWindow* _mainPanel;
    wxStaticText* _algorithmText;
    wxChoice* _algorithmChoice;
    wxStaticText* _renderingPrecisionText;
    wxChoice* _renderingPrecisionChoice;
    wxStaticText* _antiAliasingText;
    wxChoice* _antiAliasingChoice;
    wxStaticText* _optionsText;
    wxCheckBox* _relativeCheck;
    wxCheckBox* _colorFractal;
    wxCheckBox* _colorSet;
    wxCheckBox* _orbitTrap;
    wxCheckBox* _smoothRender;
    wxStaticText* _setColorText;
    wxColourPickerCtrl* _setColorPicker;
    wxButton* _closeButton;
    wxPanel* _colorPanel;
    wxStaticText* _gradStylesLabel;
    wxChoice* _gradStylesChoice;
    wxButton* _gradButton;
    wxStaticBitmap* _gradientMap;
    wxStaticText* _gradPalText;
    wxSpinCtrl* _gradPalSize;
    wxStaticText* _colorCycleText;
    wxSpinCtrl* _colorCycleLength;
    wxStaticText* _paletteMappingText;
    wxChoice* _paletteMappingMode;
    wxStaticText* _paletteMappingExponentText;
    wxSpinCtrlDouble* _paletteMappingExponent;
    wxStaticText* _colorVarText;
    wxSlider* _colorVarSlider;
    wxStaticText* _colorRotationSpeedText;
    wxSpinCtrl* _colorRotationSpeed;
    
    FractalPresenter* _fractalPresenter;            ///< Presenter used to mutate the displayed fractal.
    Fractal* _target;                               ///< Target fractal.
    sf::Color _setColor;                            ///< Color of the fractal set.
    ColorPalette _gradFractalColor;                 ///< Color in Grad color mode.
    std::function<void(const Options&)> _optionsChanged;
    std::vector<RenderingPrecisionMode> _renderingPrecisionModes;
    int _escapeTimeIndex, _gaussIntIndex, _buddhabrotIndex;
    int _escapeAngleIndex, _triangleIneqIndex, _chaoticMapIndex;
    int _lyapunovIndex, _convergenceTestIndex;

    void OnClose(wxCloseEvent& event);
    void OnChangeAlgorithm(wxCommandEvent& event);
    void OnRenderingPrecision(wxCommandEvent& event);
    void OnAntiAliasing(wxCommandEvent& event);
    void OnRelativeColor(wxCommandEvent& event);
    void OnColorFractal(wxCommandEvent& event);
    void OnColorSet(wxCommandEvent& event);
    void OnOrbitTrap(wxCommandEvent& event);
    void OnSmoothRender(wxCommandEvent& event);
    void OnSetColor(wxColourPickerEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnGrad(wxCommandEvent& event);
    void GradientColorChangeSelection(wxCommandEvent& event);
    void OnGradPaletteSize(wxSpinEvent& event);
    void OnColorCycleLength(wxSpinEvent& event);
    void OnPaletteMappingMode(wxCommandEvent& event);
    void OnPaletteMappingExponent(wxSpinDoubleEvent& event);
    void OnChangeColorVariationOffset(wxScrollEvent& event);
    void OnColorRotationSpeed(wxSpinEvent& event);

    void ConnectEvents();
    void NotifyOptionsChanged() const;
    void SyncRenderingPrecisionControl();
    void SyncAntiAliasingControl() const;
    void SyncRelativeColorControl() const;
    void SyncPaletteMappingControls() const;
    void SetAlgorithmChoices();        ///< Search for the algorithms available in the target fractal and constructs choice widget.
    [[nodiscard]] wxBitmap PaintGradient() const;    ///< Paints the gradient widget.
    static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                        const wxString& darkIcon);

public:
    ///@brief Constructor.
    ///@param presenter Pointer to the target fractal presenter.
    ///@param parent Parent window that receives renderer options lifecycle events.
    ///@param optionsChanged Callback invoked after renderer options are changed.
    ///@param id Window identifier.
    ///@param title Text displayed in the frame title bar.
    ///@param pos Initial frame position.
    ///@param size Initial frame size.
    ///@param windowStyle wxWidgets frame style flags.
    RendererOptionsFrame(FractalPresenter* presenter, wxWindow* parent,
                         std::function<void(const Options&)> optionsChanged = {}, wxWindowID id = wxID_ANY,
                         const wxString& title = wxTRANSLATE("Renderer options"),
                         const wxPoint& pos = wxDefaultPosition, const wxSize& size = RendererOptionsFrameSize,
                         long windowStyle = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
    ///@brief Sets the target fractal.
    ///@param presenter Pointer to target fractal presenter.
    void SetTarget(FractalPresenter* presenter);
    /// @brief Applies a rendering algorithm and feature flags through the same path as the option's frame.
    /// @param algorithm Rendering algorithm to select.
    /// @param smoothRender Enables smooth escape-time coloring.
    /// @param orbitTrap Enables orbit-trap coloring.
    /// @return true when the current target supports the requested options.
    bool SetRenderingOptions(RenderingAlgorithmType algorithm, bool smoothRender, bool orbitTrap);

};
