#include <wx/gdicmn.h>
#include <wx/statbox.h>
#include "ConstDialog.h"
#include "TextUtils.h"

ConstDialog::ConstDialog(bool* active, SFMLFractal* presenter, wxWindow* parent, const wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    _presenter = presenter;
    _target = _presenter->GetFractal();
    _active = active;

    this->SetSizeHints(wxSize(320, 250), wxDefaultSize);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    
    _dumbPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto sizer = new wxBoxSizer(wxVERTICAL);

    const auto realSizer = new wxStaticBoxSizer(new wxStaticBox(_dumbPanel, wxID_ANY, wxT("Real value")), wxVERTICAL);
    
    _lastReal = _target->GetKReal();
    wxString text = TextUtils::ToWxString(_target->GetKReal());
    _realText = new wxTextCtrl(_dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    realSizer->Add(_realText, 0, wxALL|wxEXPAND, 5);
    sizer->Add(realSizer, 2, wxEXPAND, 5);

    const auto imSizer = new wxStaticBoxSizer(new wxStaticBox(_dumbPanel, wxID_ANY, wxT("Imaginary value")), wxVERTICAL);
    
    _lastIm = _target->GetKImaginary();
    text = TextUtils::ToWxString(_target->GetKImaginary());
    _imText = new wxTextCtrl(_dumbPanel, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, 0);
    imSizer->Add(_imText, 0, wxALL|wxEXPAND, 5);
    
    sizer->Add(imSizer, 2, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    const auto okSizer = new wxBoxSizer(wxVERTICAL);
    
    _okButton = new wxButton(_dumbPanel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    okSizer->Add(_okButton, 0, wxALL, 5);
    buttonSizer->Add(okSizer, 1, wxEXPAND, 5);

    const auto applySizer = new wxBoxSizer(wxVERTICAL);
    
    _applyButton = new wxButton(_dumbPanel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    applySizer->Add(_applyButton, 0, wxALL, 5);
    buttonSizer->Add(applySizer, 1, wxEXPAND, 5);
    sizer->Add(buttonSizer, 1, wxEXPAND, 5);
    
    _dumbPanel->SetSizer(sizer);
    _dumbPanel->Layout();
    sizer->Fit(_dumbPanel);
    mainSizer->Add(_dumbPanel, 1, wxEXPAND | wxALL, 0);
    
    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    
    this->Centre(wxBOTH);

    _okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), nullptr, this);
    _applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnApply), nullptr, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ConstDialog::OnClose));
}

ConstDialog::~ConstDialog()
{
    _okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConstDialog::OnOk), nullptr, this);
    *_active = false;
}

void ConstDialog::OnOk(wxCommandEvent&)
{
    const double real = TextUtils::ToDouble(_realText->GetLineText(0));
    const double imag = TextUtils::ToDouble(_imText->GetLineText(0));

    if (real != _lastReal || imag != _lastIm)
        _presenter->SetK(real, imag);

    this->Close(true);
    this->Destroy();
}
void ConstDialog::OnApply(wxCommandEvent&)
{
    const double real = TextUtils::ToDouble(_realText->GetLineText(0));
    const double imag = TextUtils::ToDouble(_imText->GetLineText(0));

    _lastReal = real;
    _lastIm = imag;

    _presenter->SetK(real, imag);
}
void ConstDialog::OnClose(wxCloseEvent&)
{
    this->Destroy();
}
