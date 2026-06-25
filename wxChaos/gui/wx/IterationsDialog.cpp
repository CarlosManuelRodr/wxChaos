#include "AppPaths.h"
#include "IterationsDialog.h"
#include "TextUtils.h"

IterationsDialog::IterationsDialog(bool* Active, FractalPresenter* presenter, wxWindow* parent, wxWindowID id, const wxString& title,
                                   const wxPoint& pos, const wxSize& size, const long style)
                                   : wxFrame(parent, id, title, pos, size, style)
{
    // WX Frame.
    const wxIcon icon(AppPaths::ResourceFile({wxT("icon.ico")}), wxBITMAP_TYPE_ICO);
    this->SetIcon(icon);

    _active = Active;
    _fractalPresenter = presenter;
    _target = _fractalPresenter->GetFractal();
    this->SetSizeHints(wxSize(420, 180), wxSize(420, 180));

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto  subSizer = new wxBoxSizer(wxVERTICAL);

    const auto  textSizer = new wxStaticBoxSizer(new wxStaticBox(_panel, wxID_ANY, wxT("Iterations")), wxHORIZONTAL);

    _number = _target->GetIterations();
    _text = TextUtils::ToWxString(static_cast<int>(_number));
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

    _plusButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnPlus, this);
    _minusButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnMinus, this);
    _acceptButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnOk, this);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnApply, this);
}

IterationsDialog::~IterationsDialog()
{
    *_active = false;
    _plusButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnPlus, this);
    _minusButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnMinus, this);
    _acceptButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnOk, this);
    _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &IterationsDialog::OnApply, this);
}

void IterationsDialog::OnPlus(wxCommandEvent&)
{
    // Increases iterations.
    _number++;
    _text = TextUtils::ToWxString(static_cast<int>(_number));
    _textCtrl->SetValue(_text);
    _fractalPresenter->ChangeIterations(_number);
}
void IterationsDialog::OnMinus(wxCommandEvent&)
{
    // Decreases iterations.
    if (_number - 1 > 0)
        _number--;
    _text = TextUtils::ToWxString(static_cast<int>(_number));
    _textCtrl->SetValue(_text);
    _fractalPresenter->ChangeIterations(_number);
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

    const int value = TextUtils::ToInt(_text);;
    if (value < 0)
    {
        wxMessageBox("Negative values are not allowed.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    _number = value;
    _fractalPresenter->ChangeIterations(_number);
}
void IterationsDialog::SetTarget(FractalPresenter* presenter)
{
    _fractalPresenter = presenter;
    _target = _fractalPresenter->GetFractal();
    _textCtrl->SetValue(TextUtils::ToWxString(static_cast<int>(_target->GetIterations())));
    _number = _target->GetIterations();
}
