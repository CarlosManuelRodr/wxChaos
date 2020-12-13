#include "ConstDialog.h"
#include "StringFuncs.h"
#include "global.h"

ConstDialog::ConstDialog(bool* Active, Fractal* mTarget, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, 
                         const wxSize& size, long style) 
    : wxDialog(parent, id, title, pos, size, style)
{
    target = mTarget;
    wxString text;
    active = Active;

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    dumbPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticBoxSizer* realSizer = new wxStaticBoxSizer(new wxStaticBox(dumbPanel, wxID_ANY, wxT(realValTxt)), wxVERTICAL);    // Txt: "Real value"
    
    lastReal = target->GetKReal();
    text = num_to_string(target->GetKReal());
    realText = new wxTextCtrl(dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    realSizer->Add(realText, 0, wxALL|wxEXPAND, 5);
    sizer->Add(realSizer, 2, wxEXPAND, 5);
    
    wxStaticBoxSizer* imSizer = new wxStaticBoxSizer(new wxStaticBox(dumbPanel, wxID_ANY, wxT(imagValTxt)), wxVERTICAL);    // Txt: "Imaginary value"
    
    lastIm = target->GetKImaginary();
    text = num_to_string(target->GetKImaginary());
    imText = new wxTextCtrl(dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    imSizer->Add(imText, 0, wxALL|wxEXPAND, 5);
    
    sizer->Add(imSizer, 2, wxEXPAND, 5);
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* okSizer = new wxBoxSizer(wxVERTICAL);
    
    okButton = new wxButton(dumbPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Ok"
    okSizer->Add(okButton, 0, wxALL, 5);
    buttonSizer->Add(okSizer, 1, wxEXPAND, 5);
    
    wxBoxSizer* applySizer = new wxBoxSizer(wxVERTICAL);
    
    applyButton = new wxButton(dumbPanel, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Apply"
    applySizer->Add(applyButton, 0, wxALL, 5);
    buttonSizer->Add(applySizer, 1, wxEXPAND, 5);
    sizer->Add(buttonSizer, 1, wxEXPAND, 5);
    
    dumbPanel->SetSizer(sizer);
    dumbPanel->Layout();
    sizer->Fit(dumbPanel);
    mainSizer->Add(dumbPanel, 1, wxEXPAND | wxALL, 0);
    
    this->SetSizer(mainSizer);
    this->Layout();
    
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