/**
* @file JuliaConstantDialog.h
* @brief This header file contains the JuliaConstantDialog.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/18/2012
*/

#pragma once

#include <wx/panel.h>
#include <wx/dialog.h>
#include "sfml/FractalPresenter.h"

/**
* @class JuliaConstantDialog
* @brief Dialog that sets the constant "K" in a Julia fractal.
*/

class JuliaConstantDialog : public wxDialog
{
    wxPanel* _dumbPanel;
    wxTextCtrl* _realText;
    wxTextCtrl* _imText;
    wxButton* _okButton;
    wxButton* _applyButton;
    FractalPresenter* _presenter;
    Fractal* _target;

    double _lastReal, _lastIm;
    bool* _active;

    void OnOk(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

public:
    JuliaConstantDialog(bool* active, FractalPresenter* presenter, wxWindow* parent, wxWindowID id = wxID_ANY,
                        const wxString& title = "Enter constant", const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(320, 250), long style = wxDEFAULT_DIALOG_STYLE);
    ~JuliaConstantDialog() override;
};
