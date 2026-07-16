// ReSharper disable CppDFAMemoryLeak
#include "common/UnrestrictedSpinDoubleCtrl.h"

#include "TextUtils.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <wx/sizer.h>

UnrestrictedSpinDoubleCtrl::UnrestrictedSpinDoubleCtrl(wxWindow* parent, const wxString& value,
                                                       const double increment)
                                                       : wxPanel(parent, wxID_ANY), _increment(increment)
{
    const auto sizer = new wxBoxSizer(wxHORIZONTAL);

    _textCtrl = new wxTextCtrl(this, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, 0);
    _decrementButton = new wxButton(this, wxID_ANY, "-", wxDefaultPosition, wxSize(32, -1), 0);
    _incrementButton = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(32, -1), 0);

    sizer->Add(_textCtrl, 1, wxEXPAND);
    sizer->Add(_decrementButton, 0, wxEXPAND | wxLEFT, 3);
    sizer->Add(_incrementButton, 0, wxEXPAND | wxLEFT, 3);
    SetSizer(sizer);

    _decrementButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &UnrestrictedSpinDoubleCtrl::OnDecrement, this);
    _incrementButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &UnrestrictedSpinDoubleCtrl::OnIncrement, this);
}

double UnrestrictedSpinDoubleCtrl::GetValue() const
{
    return TextUtils::ToDouble(_textCtrl->GetValue());
}

// ReSharper disable CppMemberFunctionMayBeConst
void UnrestrictedSpinDoubleCtrl::SetValue(const double value)
{
    _textCtrl->SetValue(FormatValue(value));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UnrestrictedSpinDoubleCtrl::OnDecrement(wxCommandEvent&)
{
    AdjustValue(-_increment);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UnrestrictedSpinDoubleCtrl::OnIncrement(wxCommandEvent&)
{
    AdjustValue(_increment);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
// ReSharper disable CppMemberFunctionMayBeConst
void UnrestrictedSpinDoubleCtrl::AdjustValue(const double delta)
{
    SetValue(GetValue() + delta);
}

wxString UnrestrictedSpinDoubleCtrl::FormatValue(const double value)
{
    std::ostringstream output;
    output << std::setprecision(15) << value;

    std::string text = output.str();
    if (text.find('.') != std::string::npos)
    {
        while (!text.empty() && text.back() == '0')
            text.pop_back();

        if (!text.empty() && text.back() == '.')
            text.pop_back();
    }

    return wxString::FromUTF8(text.c_str());
}
