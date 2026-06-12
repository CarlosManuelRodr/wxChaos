/**
* @file RendererOptions.h
* @brief This header contains the RendererOptions.
*
* @author Carlos Manuel Rodriguez Martinez
*
* @date 7/19/2012
*/

#pragma once
#ifndef _color
#define _color

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <SFML/Graphics/Color.hpp>
#include <functional>
#include "ColorPalette.h"
#include "wxGradientDialog.h"
#include "Fractal.h"
#include "sfml/SFMLFractal.h"

#define RendererOptionsFrameSize wxSize(900, 820)

/**
* @class RendererOptions
* @brief Frame that allows the user to change several color and algorithm parameters.
*/
class RendererOptions : public wxFrame
{
    static constexpr int BuddhabrotPaletteSize = 1500;

    wxScrolledWindow* _mPanel;
    wxStaticText* _algorithmText;
    wxChoice* _algorithmChoice;
    wxStaticText* _optionsText;
    wxCheckBox* _relativeCheck;
    wxCheckBox* _colorFractal;
    wxCheckBox* _colorSet;
    wxCheckBox* _orbitTrap;
    wxCheckBox* _smoothRender;
    wxStaticText* _redSetText;
    wxSlider* _redSetSld;
    wxStaticText* _greenSetText;
    wxSlider* _greenSetSld;
    wxStaticText* _blueSetText;
    wxSlider* _blueSetSld;
    wxButton* _okButton;
    wxNotebook* _typeNotebook;
    wxPanel* _gradientLabel;
    wxStaticText* _gradStylesLabel;
    wxChoice* _gradStylesChoice;
    wxButton* _gradButton;
    wxStaticBitmap* _gradientMap;
    wxStaticText* _gradPalText;
    wxSpinCtrl* _gradPalSize;
    wxStaticText* _colorVarText;
    wxSlider* _colorVarSlider;
    
    SFMLFractal* _presenter;                        ///< Presenter used to mutate the displayed fractal.
    Fractal* _target;                               ///< Target fractal.
    bool* _active;                                  ///< Used to communicate with the MainFrame.
    sf::Color _setColor;                            ///< Color of the fractal set.
    ColorPalette _gradFractalColor;                 ///< Color in Grad color mode.
    std::function<void(const Options&)> _optionsChanged;
    int _escapeTimeIndex, _gaussIntIndex, _buddhabrotIndex;
    int _escapeAngleIndex, _triangleIneqIndex, _chaoticMapIndex;
    int _lyapunovIndex, _convergenceTestIndex;

    void OnClose(wxCloseEvent& event);
    void OnChangeAlgorithm(wxCommandEvent& event);
    void OnRelativeColor(wxCommandEvent& event);
    void OnColorFractal(wxCommandEvent& event);
    void OnColorSet(wxCommandEvent& event);
    void OnOrbitTrap(wxCommandEvent& event);
    void OnSmoothRender(wxCommandEvent& event);
    void OnSetRed(wxScrollEvent& event);
    void OnSetGreen(wxScrollEvent& event);
    void OnSetBlue(wxScrollEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnGrad(wxCommandEvent& event);
    void GradientColorChangeSelection(wxCommandEvent& event);
    void OnGradPaletteSize(wxSpinEvent& event);
    void OnColorVar(wxScrollEvent& event);

    void ConnectEvents();
    void NotifyOptionsChanged() const;
    void SetAlgorithmChoices();        ///< Search for the algorithms available in the target fractal and constructs choice widget.
    [[nodiscard]] int GetPaletteSizeForAlgorithm(RenderingAlgorithmType algorithm, int paletteSize) const;
    void ApplyPaletteSize(int paletteSize);
    [[nodiscard]] wxBitmap PaintGradient() const;    ///< Paints the gradient widget.


public:
    ///@brief Constructor.
    ///@param active Used to communicate with the MainFrame.
    ///@param presenter Pointer to the target fractal presenter.
    ///@param parent Parent wxWindow.
    ///@param title
    ///@param pos
    ///@param windowStyle
    RendererOptions(bool* active, SFMLFractal* presenter, wxWindow* parent,
               std::function<void(const Options&)> optionsChanged = {}, wxWindowID id = wxID_ANY, const wxString& title = wxT("Renderer options"),
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = RendererOptionsFrameSize,
               long windowStyle = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
    ~RendererOptions() override;

    ///@brief Sets the target fractal.
    ///@param presenter Pointer to target fractal presenter.
    void SetTarget(SFMLFractal* presenter);

};

#endif //_color
