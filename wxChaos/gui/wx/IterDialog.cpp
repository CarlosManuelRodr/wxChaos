#include "IterDialog.h"
#include "StringFuncs.h"
#include "Filesystem.h"
#include "global.h"

IterDialog::IterDialog(bool* Active, Fractal* _target, wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, long style)
    : wxFrame()
{
    wxXmlResource::Get()->LoadFrame(this, parent, "IterDialog");

    wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    active = Active;
    target = _target;
    this->SetSizeHints(wxSize(314, 124), wxSize(314, 124));

    panel = XRCCTRL(*this, "panel", wxPanel);
    textCtrl = XRCCTRL(*this, "textCtrl", wxTextCtrl);
    plusButton = XRCCTRL(*this, "plusButton", wxButton);
    minusButton = XRCCTRL(*this, "minusButton", wxButton);
    acceptButton = XRCCTRL(*this, "acceptButton", wxButton);
    applyButton = XRCCTRL(*this, "applyButton", wxButton);

    number = target->GetIterations();
    text = num_to_string((int)number);
    textCtrl->SetValue(text);

    this->Centre(wxBOTH);

    plusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnPlus), NULL, this);
    minusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnMinus), NULL, this);
    acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnOk), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnApply), NULL, this);
}

IterDialog::~IterDialog()
{
    *active = false;
    plusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnPlus), NULL, this);
    minusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnMinus), NULL, this);
    acceptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnOk), NULL, this);
    applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterDialog::OnApply), NULL, this);
}

void IterDialog::OnPlus(wxCommandEvent& event)
{
    // Increases iterations.
    number++;
    text = num_to_string((int)number);
    textCtrl->SetValue(text);
    target->ChangeIterations(number);
}
void IterDialog::OnMinus(wxCommandEvent& event)
{
    // Decreases iterations.
    if(number - 1 > 0)
        number--;
    text = num_to_string((int)number);
    textCtrl->SetValue(text);
    target->ChangeIterations(number);
}
void IterDialog::OnOk(wxCommandEvent& event)
{
    // Closes dialog.
    this->Close(true);
    this->Destroy();
}
void IterDialog::OnApply(wxCommandEvent& event)
{
    // Redraw fractal.
    text = textCtrl->GetValue();
    number = string_to_int(text);
    target->ChangeIterations(number);
}
void IterDialog::SetTarget(Fractal* _target)
{
    target = _target;
    textCtrl->SetValue(num_to_string((int)target->GetIterations()));
    number = target->GetIterations();
}