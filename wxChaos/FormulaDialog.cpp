#include "FormulaDialog.h"
#include "StringFuncs.h"

// FuncDialog
FuncDialog::FuncDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    // WX Dialog.
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);

    text = new wxTextCtrl(mainPanel, wxID_ANY, wxString(wxT(availableFuncTxt)) + wxT("abs(), sin(), cos(), tan(), sinh(), cosh(),\ntanh(), ln(), log(), log10(), exp(), sqrt()."),
                            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY);    // Txt: "Available functions:\n"
    text->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

    textSizer->Add(text, 1, wxALL|wxEXPAND, 5);
    panelSizer->Add(textSizer, 4, wxEXPAND, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);

    closeButton = new wxButton(mainPanel, wxID_ANY, wxT(closeTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Close"
    buttonSizer->Add(closeButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    panelSizer->Add(buttonSizer, 1, wxEXPAND, 5);

    mainPanel->SetSizer(panelSizer);
    mainPanel->Layout();
    panelSizer->Fit(mainPanel);
    mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    this->Layout();

    this->Centre(wxBOTH);

    closeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FuncDialog::OnClose), NULL, this);
}
FuncDialog::~FuncDialog()
{
    closeButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FuncDialog::OnClose), NULL, this);
}
void FuncDialog::OnClose(wxCommandEvent& event)
{
    this->Close(true);
}

// FormulaDialog
FormulaDialog::FormulaDialog(int _userDefinedID, int _FPuserDefinedID, GaussianColorStyles* mColorStyle, wxMenuItem* juliaSlider, wxMenuItem* juliaManual,
                                bool *Active, FractalCanvas* _fCanvas, wxWindow* _parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
                            : wxDialog(_parent, id, title, pos, size, style)
{
    // WX Dialog.
    userDefinedID = _userDefinedID;
    FPuserDefinedID = _FPuserDefinedID;

    parent = _parent;
    this->SetSizeHints(FormulaDialogSize, wxDefaultSize);
    fCanvas = _fCanvas;
    active = Active;
    colorStyle = mColorStyle;

    slider = juliaSlider;
    manual = juliaManual;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* formulaSizer = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT(formulaTxt)), wxVERTICAL);    // Txt: "Formula"

    formulaCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxString(fCanvas->GetFormula().userFormula), wxDefaultPosition, wxDefaultSize, 0);
    formulaSizer->Add(formulaCtrl, 2, wxALL | wxEXPAND, 5);
    panelSizer->Add(formulaSizer, 3, wxEXPAND, 5);

    wxStaticBoxSizer* optionSizer = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT(optionsTxt)), wxHORIZONTAL);
    wxBoxSizer* bailoutSizer = new wxBoxSizer(wxVERTICAL);

    bailText = new wxStaticText(mainPanel, wxID_ANY, wxT(bailValueTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Bailout value"
    bailText->Wrap(-1);
    bailoutSizer->Add(bailText, 0, wxALL, 5);

    bailCtrl = new wxTextCtrl(mainPanel, wxID_ANY, num_to_string(fCanvas->GetFormula().bailout), wxDefaultPosition, wxDefaultSize, 0);
    bailoutSizer->Add(bailCtrl, 0, wxALL|wxEXPAND, 5);
    optionSizer->Add(bailoutSizer, 1, wxEXPAND, 5);

    wxBoxSizer* typeSizer = new wxBoxSizer(wxVERTICAL);

    typeText = new wxStaticText(mainPanel, wxID_ANY, wxT(fractTypeTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Fractal type"
    typeText->Wrap(-1);
    typeSizer->Add(typeText, 0, wxALL, 5);

    wxString typeChoiceChoices[] = { wxT(complexTypeTxt), wxT(fixedPointTypeTxt) };
    int typeChoiceNChoices = sizeof( typeChoiceChoices ) / sizeof( wxString );
    typeChoice = new wxChoice(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, typeChoiceNChoices, typeChoiceChoices, 0);

    typeSizer->Add(typeChoice, 0, wxALL | wxEXPAND, 5);

    juliaCheck = new wxCheckBox(mainPanel, wxID_ANY, wxT(juliaTypeTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Julia type"
    typeSizer->Add(juliaCheck, 0, wxALL, 5);

    if(fCanvas->GetFormula().julia)
        juliaCheck->SetValue(true);
    else
        juliaCheck->SetValue(false);

    optionSizer->Add(typeSizer, 1, wxEXPAND, 5);
    panelSizer->Add(optionSizer, 3, wxEXPAND, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    acceptButton = new wxButton(mainPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(acceptButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    applyButton = new wxButton(mainPanel, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(applyButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    funcButton = new wxButton( mainPanel, wxID_ANY, wxT(availableLabelTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Available functions"
    buttonSizer->Add( funcButton, 0, wxALL, 5 );
    panelSizer->Add(buttonSizer, 1, wxEXPAND, 5);

    mainPanel->SetSizer(panelSizer);
    mainPanel->Layout();
    panelSizer->Fit(mainPanel);
    sizer->Add(mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(sizer);
    this->Layout();
    this->Centre(wxBOTH);

    if(fCanvas->GetFormula().type == FormulaType::Complex)
    {
        typeChoice->SetSelection( 0 );
        juliaCheck->Enable(true);
        bailCtrl->Enable(true);
    }
    else
    {
        typeChoice->SetSelection( 1 );
        juliaCheck->Enable(false);
        bailCtrl->Enable(false);
    }

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FormulaDialog::OnClose));
    typeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(FormulaDialog::OnChoice), NULL, this);
    acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnAccept), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnApply), NULL, this);
    funcButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnFunc), NULL, this);
}

FormulaDialog::~FormulaDialog()
{
    typeChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(FormulaDialog::OnChoice), NULL, this);
    acceptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnAccept), NULL, this);
    applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnApply), NULL, this);
    funcButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnFunc), NULL, this);
}
void FormulaDialog::OnAccept(wxCommandEvent& event)
{
    // Closes window.
    this->Show(false);
    *active = false;
    this->Destroy();
}
void FormulaDialog::OnApply(wxCommandEvent& event)
{
    // Creates fractal with the formula.
    FormulaOpt options;
    options.userFormula = formulaCtrl->GetValue();
    options.bailout = string_to_int(bailCtrl->GetValue());
    options.julia = juliaCheck->GetValue();

    if(typeChoice->GetCurrentSelection() == 0)
        options.type = FormulaType::Complex;
    else
        options.type = FormulaType::FixedPoint;

    fCanvas->SetUserFormula(options);

    if(typeChoice->GetCurrentSelection() == 0)
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, userDefinedID);
        event.SetEventObject(parent);
        parent->GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, FPuserDefinedID);
        event.SetEventObject(parent);
        parent->GetEventHandler()->ProcessEvent(event);
    }
}
void FormulaDialog::OnClose(wxCloseEvent& event)
{
    this->Show(false);
    *active = false;
    this->Destroy();
}
void FormulaDialog::OnChoice( wxCommandEvent& event )
{
    if(typeChoice->GetCurrentSelection() == 0)
    {
        juliaCheck->Enable(true);
        bailCtrl->Enable(true);
        formulaCtrl->SetValue(wxT("z = z^2 + c"));
    }
    else
    {
        juliaCheck->Enable(false);
        bailCtrl->Enable(false);
        formulaCtrl->SetValue(wxT("z = sin(z)"));
    }
}
void FormulaDialog::OnFunc(wxCommandEvent& event)
{
    FuncDialog* diag = new FuncDialog(this, wxID_ANY);
    diag->Move(this->GetPosition().x + this->GetSize().x, this->GetPosition().y);
    diag->Show(true);
}