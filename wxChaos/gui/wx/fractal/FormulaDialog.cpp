#include "fractal/FormulaDialog.h"

#include "AppPaths.h"
#include "common/AppTheme.h"
#include "fractal/FunctionsHelpDialog.h"
#include "TextUtils.h"

wxPanel* FormulaDialog::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                            const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(24, 24);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, CreateIconBundle(lightIcon, darkIcon, iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 10);

    const auto title = new wxStaticText(header, wxID_ANY, text);
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(titleFont.GetPointSize() + 1);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    title->SetBackgroundColour(AppTheme::ControlBackground());
    title->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 48));
    return header;
}

wxBitmapBundle FormulaDialog::CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                               const wxSize& size)
{
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), size);
}

FormulaDialog::FormulaDialog(const int userDefinedId, const int fPUserDefinedId, const int newtonUserDefinedId, wxMenuItem* juliaSlider,
                             wxMenuItem* juliaManual, bool* active, FractalCanvas* fCanvas, wxWindow* parent,
                             const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                             const long style) : wxDialog(parent, id, title, pos, size, style)
{
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
    const auto formulaSizer = new wxBoxSizer(wxVERTICAL);
    formulaSizer->Add(CreateSectionHeader(_mainPanel, "Formula", "formula_light.svg", "formula_dark.svg"),
                      0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _formulaCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, wxString(_fCanvas->GetFormula().userFormula), wxDefaultPosition, wxDefaultSize, 0);
    formulaSizer->Add(_formulaCtrl, 0, wxALL | wxEXPAND, 10);
    panelSizer->Add(formulaSizer, 0, wxEXPAND, 5);

    const auto optionSizer = new wxStaticBoxSizer(new wxStaticBox(_mainPanel, wxID_ANY, "Options"), wxHORIZONTAL);
    const auto bailoutSizer = new wxBoxSizer(wxVERTICAL);

    _bailText = new wxStaticText(_mainPanel, wxID_ANY, "Bailout value:", wxDefaultPosition, wxDefaultSize, 0);
    _bailText->Wrap(-1);
    bailoutSizer->Add(_bailText, 0, wxALL, 5);

    _bailCtrl = new wxTextCtrl(_mainPanel, wxID_ANY, TextUtils::ToWxString(_fCanvas->GetFormula().bailout), wxDefaultPosition, wxDefaultSize, 0);
    bailoutSizer->Add(_bailCtrl, 0, wxALL|wxEXPAND, 5);
    optionSizer->Add(bailoutSizer, 1, wxEXPAND, 5);

    const auto typeSizer = new wxBoxSizer(wxVERTICAL);

    _typeText = new wxStaticText(_mainPanel, wxID_ANY, "Fractal type:", wxDefaultPosition, wxDefaultSize, 0);
    _typeText->Wrap(-1);
    typeSizer->Add(_typeText, 0, wxALL, 5);

    const wxString typeChoiceChoices[] = { "Escape time", "Fixed point", "Newton-Raphson" };
    constexpr int typeChoiceNChoices = sizeof(typeChoiceChoices) / sizeof(wxString);
    _typeChoice = new wxChoice(_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, typeChoiceNChoices, typeChoiceChoices, 0);

    typeSizer->Add(_typeChoice, 0, wxALL | wxEXPAND, 5);

    _juliaCheck = new wxCheckBox(_mainPanel, wxID_ANY, "Julia type", wxDefaultPosition, wxDefaultSize, 0);
    typeSizer->Add(_juliaCheck, 0, wxALL, 5);

    _juliaCheck->SetValue(_fCanvas->GetFormula().julia);

    optionSizer->Add(typeSizer, 1, wxEXPAND, 5);
    panelSizer->Add(optionSizer, 0, wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    _acceptButton = new wxButton(_mainPanel, wxID_ANY, "Ok", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_acceptButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    _applyButton = new wxButton(_mainPanel, wxID_ANY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_applyButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    _funcButton = new wxButton( _mainPanel, wxID_ANY, "Available functions", wxDefaultPosition, wxDefaultSize, 0 );
    buttonSizer->Add( _funcButton, 0, wxALL, 5 );
    panelSizer->Add(buttonSizer, 0, wxEXPAND, 5);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    sizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    sizer->Fit(this);
    this->wxTopLevelWindowBase::SetMinSize(this->GetSize());
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
        _formulaCtrl->SetValue("z = z^2 + c");
    }
    else
    {
        _juliaCheck->Enable(false);
        _bailCtrl->Enable(false);
        if (_typeChoice->GetCurrentSelection() == 1)
            _formulaCtrl->SetValue("z = sin(z)");
        else
            _formulaCtrl->SetValue("z^3 - 1");
    }
}
void FormulaDialog::OnFunc(wxCommandEvent&)
{
    const auto diag = new FunctionsHelpDialog(this, wxID_ANY);
    diag->Move(this->GetPosition().x + this->GetSize().x, this->GetPosition().y);
    diag->Show(true);
}
