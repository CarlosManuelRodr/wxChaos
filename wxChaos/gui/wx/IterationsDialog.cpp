#include "IterationsDialog.h"
#include "StringFuncs.h"
#include "Filesystem.h"

IterationsDialog::IterationsDialog(bool* Active, SFMLFractal* presenter, wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    // WX Frame.
    const wxIcon icon(GetWxAbsPath({ "Resources", "icon.ico" }), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    _active = Active;
    _presenter = presenter;
    _target = _presenter->GetFractal();
    this->SetSizeHints(wxSize(420, 180), wxSize(420, 180));

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto  subSizer = new wxBoxSizer(wxVERTICAL);

    const auto  textSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, wxT("Iterations")), wxHORIZONTAL);

    _number = _target->GetIterations();
    _text = num_to_string(static_cast<int>(_number));
    _textCtrl = new wxTextCtrl(_panel, wxID_ANY, _text, wxDefaultPosition, wxDefaultSize, 0);
    textSizer->Add(_textCtrl, 0, wxALL, 5);

    _plusButton = new wxButton(_panel, wxID_ANY, wxT("+"), wxDefaultPosition, wxDefaultSize, 0);
    textSizer->Add(_plusButton, 0, wxALL, 5);

    _minusButton = new wxButton(_panel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0);
    textSizer->Add(_minusButton, 0, wxALL, 5);
    subSizer->Add(textSizer, 1, wxEXPAND, 5);

    const auto  buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    _acceptButton = new wxButton(_panel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_acceptButton, 0, wxALL, 5);

    _applyButton = new wxButton(_panel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_applyButton, 0, wxALL, 5);
    subSizer->Add(buttonSizer, 0, 0, 5);

    _panel->SetSizer(subSizer);
    _panel->Layout();
    subSizer->Fit(_panel);
    sizer->Add(_panel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    _plusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnPlus), nullptr, this);
    _minusButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnMinus), nullptr, this);
    _acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnOk), nullptr, this);
    _applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnApply), nullptr, this);
}

IterationsDialog::~IterationsDialog()
{
    *_active = false;
    _plusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnPlus), nullptr, this);
    _minusButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnMinus), nullptr, this);
    _acceptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnOk), nullptr, this);
    _applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(IterationsDialog::OnApply), nullptr, this);
}

void IterationsDialog::OnPlus(wxCommandEvent&)
{
    // Increases iterations.
    _number++;
    _text = num_to_string(static_cast<int>(_number));
    _textCtrl->SetValue(_text);
    _presenter->ChangeIterations(_number);
}
void IterationsDialog::OnMinus(wxCommandEvent&)
{
    // Decreases iterations.
    if (_number - 1 > 0)
        _number--;
    _text = num_to_string(static_cast<int>(_number));
    _textCtrl->SetValue(_text);
    _presenter->ChangeIterations(_number);
}
void IterationsDialog::OnOk(wxCommandEvent&)
{
    // Closes dialog.
    this->Close(true);
    this->Destroy();
}
void IterationsDialog::OnApply(wxCommandEvent&)
{
    // Redraw fractal.
    _text = _textCtrl->GetValue();
    _number = string_to_int(_text);
    _presenter->ChangeIterations(_number);
}
void IterationsDialog::SetTarget(SFMLFractal* presenter)
{
    _presenter = presenter;
    _target = _presenter->GetFractal();
    _textCtrl->SetValue(num_to_string(static_cast<int>(_target->GetIterations())));
    _number = _target->GetIterations();
}
