#pragma once

#include <vector>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include "Fractal.h"

#define DimensionFrameSize wxSize(1200, 1260)

class ConfigFractalOptionsDialog : public wxDialog
{
    wxScrolledWindow* _mainScroll;
    wxBoxSizer* _optionsBox;
    wxStaticText* _kRealLabel;
    wxTextCtrl* _kRealCtrl;
    wxStaticText* _kImaginaryLabel;
    wxTextCtrl* _kImaginaryCtrl;
    wxStaticLine* _staticLine;
    wxButton* _okButton;
    wxButton* _applyButton;
    Fractal* _target;

    std::vector<int> _foundLabels, _foundTextControls;
    std::vector<int> _foundSpinControls, _foundCheckBoxes;
    std::vector<wxStaticText*> _labels;
    std::vector<wxTextCtrl*> _textControls;
    std::vector<wxSpinCtrl*> _spinControls;
    std::vector<wxCheckBox*> _checkBoxes;

    void OnOk(wxCommandEvent&);
    void OnApply(wxCommandEvent&);
    void AdjustOptPanel();
    void DeleteOptPanel();

public:
    ConfigFractalOptionsDialog(Fractal* target, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Fractal options",
                               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(420, 560), long style = wxCAPTION);
    ~ConfigFractalOptionsDialog() override;

    void SetNewTarget(Fractal* target);
};
