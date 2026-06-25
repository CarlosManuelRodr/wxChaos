#include "FormulaDialog.h"
#include "TextUtils.h"

// FuncDialog
FunctionsHelpDialog::FunctionsHelpDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                       const wxSize& size, const long style) : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    this->SetSizeHints(wxSize(460, 250), wxDefaultSize);

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    const auto textSizer = new wxBoxSizer(wxVERTICAL);

    _text = new wxTextCtrl(_mainPanel, wxID_ANY, wxString(wxT("Available functions:\n")) + wxT("abs(), sin(), cos(), tan(), sinh(), cosh(),\ntanh(), ln(), log(), log10(), exp(), sqrt()."),
                            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY);
    _text->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

    textSizer->Add(_text, 1, wxALL|wxEXPAND, 5);
    panelSizer->Add(textSizer, 4, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxVERTICAL);

    _closeButton = new wxButton(_mainPanel, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    panelSizer->Add(buttonSizer, 1, wxEXPAND, 5);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    mainSizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();

    this->Centre(wxBOTH);

    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FunctionsHelpDialog::OnClose, this);
}
FunctionsHelpDialog::~FunctionsHelpDialog()
{
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FunctionsHelpDialog::OnClose, this);
}
void FunctionsHelpDialog::OnClose(wxCommandEvent&)
{
    this->Close(true);
}

// FormulaDialog
FormulaDialog::FormulaDialog(const int userDefinedId, const int fPUserDefinedId, const int newtonUserDefinedId, wxMenuItem* juliaSlider,
                             wxMenuItem* juliaManual, bool* active, FractalCanvas* fCanvas, wxWindow* parent,
                             const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                             const long style) : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    _userDefinedId = userDefinedId;
    _fpUserDefinedId = fPUserDefinedId;
    _newtonUserDefinedId = newtonUserDefinedId;

    _parent = parent;
    this->SetSizeHints(FormulaDialogSize, wxDefaultSize);
    _fCanvas = fCanvas;
    _active = active;

    _slider = juliaSlider;
    _manual = juliaManual;

    const auto sizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    const auto formulaSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Formula")), wxVERTICAL);

    _formulaCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxString(_fCanvas->GetFormula().userFormula), wxDefaultPosition, wxDefaultSize, 0);
    formulaSizer->Add(_formulaCtrl, 2, wxALL | wxEXPAND, 5);
    panelSizer->Add(formulaSizer, 3, wxEXPAND, 5);

    const auto optionSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, wxT("Options")), wxHORIZONTAL);
    const auto bailoutSizer = new wxBoxSizer(wxVERTICAL);

    _bailText = new wxStaticText(_mainPanel, wxID_ANY, wxT("Bailout value:"), wxDefaultPosition, wxDefaultSize, 0);
    _bailText->Wrap(-1);
    bailoutSizer->Add(_bailText, 0, wxALL, 5);

    _bailCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, TextUtils::ToWxString(_fCanvas->GetFormula().bailout), wxDefaultPosition, wxDefaultSize, 0);
    bailoutSizer->Add(_bailCtrl, 0, wxALL|wxEXPAND, 5);
    optionSizer->Add(bailoutSizer, 1, wxEXPAND, 5);

    const auto typeSizer = new wxBoxSizer(wxVERTICAL);

    _typeText = new wxStaticText(_mainPanel, wxID_ANY, wxT("Fractal type:"), wxDefaultPosition, wxDefaultSize, 0);
    _typeText->Wrap(-1);
    typeSizer->Add(_typeText, 0, wxALL, 5);

    const wxString typeChoiceChoices[] = { wxT("Complex"), wxT("Fixed point"), wxT("Newton-Raphson") };
    constexpr int typeChoiceNChoices = sizeof(typeChoiceChoices) / sizeof(wxString);
    _typeChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, typeChoiceNChoices, typeChoiceChoices, 0);

    typeSizer->Add(_typeChoice, 0, wxALL | wxEXPAND, 5);

    _juliaCheck = new wxCheckBox(_mainPanel, wxID_ANY, wxT("Julia type"), wxDefaultPosition, wxDefaultSize, 0);
    typeSizer->Add(_juliaCheck, 0, wxALL, 5);

    _juliaCheck->SetValue(_fCanvas->GetFormula().julia);

    optionSizer->Add(typeSizer, 1, wxEXPAND, 5);
    panelSizer->Add(optionSizer, 3, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    _acceptButton = new wxButton(_mainPanel, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_acceptButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    _applyButton = new wxButton(_mainPanel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_applyButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    _funcButton = new wxButton( _mainPanel, wxID_ANY, wxT("Available functions"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( _funcButton, 0, wxALL, 5 );
    panelSizer->Add(buttonSizer, 1, wxEXPAND, 5);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    sizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    if (_fCanvas->GetFormula().type == FormulaType::Complex)
    {
        _typeChoice->SetSelection( 0 );
        _juliaCheck->Enable(true);
        _bailCtrl->Enable(true);
    }
    else if (_fCanvas->GetFormula().type == FormulaType::FixedPoint)
    {
        _typeChoice->SetSelection( 1 );
        _juliaCheck->Enable(false);
        _bailCtrl->Enable(false);
    }
    else
    {
        _typeChoice->SetSelection( 2 );
        _juliaCheck->Enable(false);
        _bailCtrl->Enable(false);
    }

    this->Bind(wxEVT_CLOSE_WINDOW, &FormulaDialog::OnClose, this);
    _typeChoice->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &FormulaDialog::OnChoice, this);
    _acceptButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnAccept, this);
    _applyButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnApply, this);
    _funcButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnFunc, this);
}

FormulaDialog::~FormulaDialog()
{
    _typeChoice->Unbind(wxEVT_COMMAND_CHOICE_SELECTED, &FormulaDialog::OnChoice, this);
    _acceptButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnAccept, this);
    _applyButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnApply, this);
    _funcButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &FormulaDialog::OnFunc, this);
}
void FormulaDialog::OnAccept(wxCommandEvent&)
{
    // Closes window.
    this->Show(false);
    *_active = false;
    this->Destroy();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void FormulaDialog::OnApply(wxCommandEvent&)
{
    // Creates fractal with the formula.
    FormulaOptions options;
    options.userFormula = _formulaCtrl->GetValue();
    options.bailout = TextUtils::ToInt(_bailCtrl->GetValue());
    options.julia = _juliaCheck->GetValue();

    if (_typeChoice->GetCurrentSelection() == 0)
        options.type = FormulaType::Complex;
    else if (_typeChoice->GetCurrentSelection() == 1)
        options.type = FormulaType::FixedPoint;
    else
        options.type = FormulaType::NewtonRaphson;

    _fCanvas->SetUserFormula(options);

    if (_typeChoice->GetCurrentSelection() == 0)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, _userDefinedId);
        event.SetEventObject(_parent);
        _parent->GetEventHandler()->ProcessEvent(event);
    }
    else if (_typeChoice->GetCurrentSelection() == 1)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, _fpUserDefinedId);
        event.SetEventObject(_parent);
        _parent->GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, _newtonUserDefinedId);
        event.SetEventObject(_parent);
        _parent->GetEventHandler()->ProcessEvent(event);
    }
}
void FormulaDialog::OnClose(wxCloseEvent&)
{
    this->Show(false);
    *_active = false;
    this->Destroy();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void FormulaDialog::OnChoice(wxCommandEvent&)
{
    if (_typeChoice->GetCurrentSelection() == 0)
    {
        _juliaCheck->Enable(true);
        _bailCtrl->Enable(true);
        _formulaCtrl->SetValue(wxT("z = z^2 + c"));
    }
    else
    {
        _juliaCheck->Enable(false);
        _bailCtrl->Enable(false);
        if (_typeChoice->GetCurrentSelection() == 1)
            _formulaCtrl->SetValue(wxT("z = sin(z)"));
        else
            _formulaCtrl->SetValue(wxT("z^3 - 1"));
    }
}
void FormulaDialog::OnFunc(wxCommandEvent&)
{
    const auto diag = new FunctionsHelpDialog(this, wxID_ANY);
    diag->Move(this->GetPosition().x + this->GetSize().x, this->GetPosition().y);
    diag->Show(true);
}
