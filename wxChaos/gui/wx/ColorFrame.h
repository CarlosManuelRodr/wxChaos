/**
* @file ColorFrame.h
* @brief This header contains the ColorFrame.
*
* @author Carlos Manuel Rodriguez y Martinez
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
#include "ColorPalettes.h"
#include "gradientdlg.h"
#include "Fractal.h"

#define ColorFrameSize wxSize(900, 820)

/**
* @class ColorFrame
* @brief Frame that allows the user to change several color and algorithm parameters.
*/
class ColorFrame : public wxFrame
{
    wxScrolledWindow* mPanel;
    wxStaticBitmap* colorOptBitmap;
    wxStaticText* algorithmText;
    wxChoice* algorithmChoice;
    wxStaticText* optionsText;
    wxCheckBox* relativeCheck;
    wxCheckBox* colorFractal;
    wxCheckBox* colorSet;
    wxCheckBox* orbitTrap;
    wxCheckBox* smoothRender;
    wxStaticText* redSetText;
    wxSlider* redSetSld;
    wxStaticText* greenSetText;
    wxSlider* greenSetSld;
    wxStaticText* blueSetText;
    wxSlider* blueSetSld;
    wxButton* okButton;
    wxNotebook* typeNotebook;
    wxPanel* gradientLabel;
    wxStaticText* gradStylesLabel;
    wxChoice* gradStylesChoice;
    wxButton* gradButton;
    wxStaticBitmap* gradientMap;
    wxStaticText* gradPalText;
    wxSpinCtrl* gradPalSize;
    wxStaticText* colorVarText;
    wxSlider* colorVarSlider;
    
    Fractal* target;                               ///< Target fractal.
    bool* active;                                  ///< Used to communicate with the MainFrame.
    sf::Color setColor;                            ///< Color of the fractal set.
    ColorPalette gradFractalColor;         ///< Color in Grad color mode.
    int escapeTimeIndex, gaussIntIndex, buddhabrotIndex;
    int escapeAngleIndex, triangleIneqIndex, chaoticMapIndex;
    int lyapunovIndex, convergenceTestIndex;

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
    void SetAlgorithmChoices();        ///< Search for the algorithms available in the target fractal and constructs choice widget.
    wxBitmap PaintGradient();          ///< Paints the gradient widget.


public:
    ///@brief Constructor.
    ///@param _active Used to communicate with the MainFrame.
    ///@param _target Pointer to the target fractal.
    ///@param parent Parent wxWindow.
    ColorFrame(bool* _active, Fractal* _target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Color options"),
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = ColorFrameSize, 
               long windowStyle = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxTAB_TRAVERSAL | wxRESIZE_BORDER);
    ~ColorFrame();

    ///@brief Sets the target fractal.
    ///@param _target Pointer to target fractal.
    void SetTarget(Fractal* _target);

};

#endif //_color
