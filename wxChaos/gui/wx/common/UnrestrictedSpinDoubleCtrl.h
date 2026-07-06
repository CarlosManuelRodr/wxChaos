#pragma once

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

class UnrestrictedSpinDoubleCtrl : public wxPanel
{
    wxTextCtrl* _textCtrl{};
    wxButton* _decrementButton{};
    wxButton* _incrementButton{};
    double _increment{};

    void OnDecrement(wxCommandEvent& event);
    void OnIncrement(wxCommandEvent& event);
    void AdjustValue(double delta) const;
    static wxString FormatValue(double value);

public:
    UnrestrictedSpinDoubleCtrl(wxWindow* parent, const wxString& value, double increment);

    [[nodiscard]] double GetValue() const;
    void SetValue(double value) const;
};
