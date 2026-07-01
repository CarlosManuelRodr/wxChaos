/** 
* @file IterationsDialog.h
* @brief Defines a dialog to change iterations.
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/19/2012
*/

#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "Fractal.h"
#include "sfml/FractalPresenter.h"

/**
* @class IterationsDialog
* @brief A dialog that lets the user manually change the iterations.
*/
class IterationsDialog : public wxFrame
{
    wxPanel* _panel;
    wxSpinCtrl* _iterationsSpinCtrl;
    wxSlider* _iterationsSlider;
    wxPanel* _scalePanel;
    wxButton* _acceptButton;
    wxButton* _applyButton;

    unsigned int _number;
    FractalPresenter* _fractalPresenter;
    Fractal* _target;
    bool* _active;
    wxWindow* _focusAfterClose;

    [[nodiscard]] static int IterationsToSliderValue(unsigned int iterations);
    [[nodiscard]] static unsigned int SliderValueToIterations(int sliderValue);
    void SetIterationControls(unsigned int iterations);
    bool ReadIterationValue(unsigned int& iterations) const;
    void OnScalePaint(wxPaintEvent& event);
    void RestoreFocusAfterClose() const;
    
public:
    IterationsDialog(bool* Active, FractalPresenter* presenter, wxWindow* parent, wxWindow* focusAfterClose = nullptr,
                     wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 340),
                     long style = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL);

    ~IterationsDialog() override;

    void OnSlider(wxCommandEvent& event);
    void OnSpin(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void SetTarget(FractalPresenter* presenter);
};
