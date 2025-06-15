#include "ConstDialog.h"
#include "StringFuncs.h"
#include "global.h"

ConstDialog::ConstDialog(bool* Active, Fractal* mTarget, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                         const wxSize& size, long style)
    : wxDialog()
{
    wxXmlResource::Get()->LoadDialog(this, parent, "ConstDialog");

    target = mTarget;
    wxString text;
    active = Active;

    dumbPanel = XRCCTRL(*this, "dumbPanel", wxPanel);
    realText = XRCCTRL(*this, "realText", wxTextCtrl);
    imText = XRCCTRL(*this, "imText", wxTextCtrl);
    okButton = XRCCTRL(*this, "okButton", wxButton);
    applyButton = XRCCTRL(*this, "applyButton", wxButton);

    lastReal = target->GetKReal();
    text = num_to_string(target->GetKReal());
    realText->SetValue(text);

    lastIm = target->GetKImaginary();
    text = num_to_string(target->GetKImaginary());
    imText->SetValue(text);

    this->Centre(wxBOTH);

    okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnApply), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ConstDialog::OnClose));
}

ConstDialog::~ConstDialog()
{
    okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), NULL, this);
    *active = false;
}

void ConstDialog::OnOk(wxCommandEvent& event)
{
    double real = string_to_double(realText->GetLineText(0));
    double imag = string_to_double(imText->GetLineText(0));

    if(real != lastReal || imag != lastIm)
        target->SetK(real, imag);

    this->Close(true);
    this->Destroy();
}
void ConstDialog::OnApply(wxCommandEvent& event)
{
    double real = string_to_double(realText->GetLineText(0));
    double imag = string_to_double(imText->GetLineText(0));

    lastReal = real;
    lastIm = imag;

    target->SetK(real, imag);
}
void ConstDialog::OnClose(wxCloseEvent& event)
{
    this->Destroy();
}
