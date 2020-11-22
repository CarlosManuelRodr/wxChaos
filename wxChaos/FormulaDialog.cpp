#include "FormulaDialog.h"
#include "StringFuncs.h"

// FuncDialog
FuncDialog::FuncDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxDialog( parent, id, title, pos, size, style )
{
    // WX Dialog.
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* mainBoxxy;
    mainBoxxy = new wxBoxSizer( wxVERTICAL );

    mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* textBoxxy;
    textBoxxy = new wxBoxSizer( wxVERTICAL );

    text = new wxTextCtrl( mainPanel, wxID_ANY, wxString(wxT(availableFuncTxt)) + wxT("abs(), sin(), cos(), tan(), sinh(), cosh(),\ntanh(), ln(), log(), log10(), exp(), sqrt()."),
                            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY );    // Txt: "Available functions:\n"
    text->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );

    textBoxxy->Add( text, 1, wxALL|wxEXPAND, 5 );

    panelBoxxy->Add( textBoxxy, 4, wxEXPAND, 5 );

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer( wxVERTICAL );

    closeButton = new wxButton( mainPanel, wxID_ANY, wxT(closeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Close"
    buttonBoxxy->Add( closeButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    panelBoxxy->Add( buttonBoxxy, 1, wxEXPAND, 5 );

    mainPanel->SetSizer( panelBoxxy );
    mainPanel->Layout();
    panelBoxxy->Fit( mainPanel );
    mainBoxxy->Add( mainPanel, 1, wxEXPAND | wxALL, 1 );

    this->SetSizer( mainBoxxy );
    this->Layout();

    this->Centre( wxBOTH );

    closeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FuncDialog::OnClose ), NULL, this );
}
FuncDialog::~FuncDialog()
{
    closeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FuncDialog::OnClose ), NULL, this );
}
void FuncDialog::OnClose(wxCommandEvent& event)
{
    this->Close(true);
}

// FormulaDialog
FormulaDialog::FormulaDialog(int _userDefinedID, int _FPuserDefinedID, EST_STYLES* mColorStyle, wxMenuItem* juliaSlider, wxMenuItem* juliaManual,
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

    wxBoxSizer* boxxy;
    boxxy = new wxBoxSizer(wxVERTICAL);

    mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* panelBoxxy;
    panelBoxxy = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* formulaBoxxy;
    formulaBoxxy = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT(formulaTxt)), wxVERTICAL);    // Txt: "Formula"

#if wxCHECK_VERSION(2, 9, 0)
    formulaCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxString(fCanvas->GetFormula().userFormula), wxDefaultPosition, wxDefaultSize, 0);
#else
    formulaCtrl = new wxTextCtrl(mainPanel, wxID_ANY, wxString(fCanvas->GetFormula().userFormula.c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, 0);
#endif
    formulaBoxxy->Add(formulaCtrl, 2, wxALL|wxEXPAND, 5);

    panelBoxxy->Add(formulaBoxxy, 3, wxEXPAND, 5);

    wxStaticBoxSizer* optionBoxxy;
    optionBoxxy = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, wxT(optionsTxt)), wxHORIZONTAL);

    wxBoxSizer* bailoutBoxxy;
    bailoutBoxxy = new wxBoxSizer(wxVERTICAL);

    bailText = new wxStaticText(mainPanel, wxID_ANY, wxT(bailValueTxt), wxDefaultPosition, wxDefaultSize, 0);    // Txt: "Bailout value"
    bailText->Wrap(-1);
    bailoutBoxxy->Add(bailText, 0, wxALL, 5);

    bailCtrl = new wxTextCtrl(mainPanel, wxID_ANY, num_to_string(fCanvas->GetFormula().bailout), wxDefaultPosition, wxDefaultSize, 0);
    bailoutBoxxy->Add(bailCtrl, 0, wxALL|wxEXPAND, 5);

    optionBoxxy->Add(bailoutBoxxy, 1, wxEXPAND, 5);

    wxBoxSizer* typeBoxxy;
    typeBoxxy = new wxBoxSizer( wxVERTICAL );

    typeText = new wxStaticText( mainPanel, wxID_ANY, wxT(fractTypeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Fractal type"
    typeText->Wrap( -1 );
    typeBoxxy->Add( typeText, 0, wxALL, 5 );

    wxString typeChoiceChoices[] = { wxT(complexTypeTxt), wxT(fixedPointTypeTxt) };
    int typeChoiceNChoices = sizeof( typeChoiceChoices ) / sizeof( wxString );
    typeChoice = new wxChoice( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, typeChoiceNChoices, typeChoiceChoices, 0 );

    typeBoxxy->Add( typeChoice, 0, wxALL|wxEXPAND, 5 );

    juliaCheck = new wxCheckBox( mainPanel, wxID_ANY, wxT(juliaTypeTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Julia type"
    typeBoxxy->Add( juliaCheck, 0, wxALL, 5 );
    if(fCanvas->GetFormula().julia) juliaCheck->SetValue(true);
    else juliaCheck->SetValue(false);

    optionBoxxy->Add(typeBoxxy, 1, wxEXPAND, 5);

    panelBoxxy->Add(optionBoxxy, 3, wxEXPAND, 5);

    wxBoxSizer* buttonBoxxy;
    buttonBoxxy = new wxBoxSizer(wxHORIZONTAL);

    acceptButton = new wxButton(mainPanel, wxID_ANY, wxT(okTxt), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(acceptButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    applyButton = new wxButton(mainPanel, wxID_ANY, wxT(applyTxt), wxDefaultPosition, wxDefaultSize, 0);
    buttonBoxxy->Add(applyButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    funcButton = new wxButton( mainPanel, wxID_ANY, wxT(availableLabelTxt), wxDefaultPosition, wxDefaultSize, 0 );    // Txt: "Available functions"
    buttonBoxxy->Add( funcButton, 0, wxALL, 5 );

    panelBoxxy->Add(buttonBoxxy, 1, wxEXPAND, 5);

    mainPanel->SetSizer(panelBoxxy);
    mainPanel->Layout();
    panelBoxxy->Fit(mainPanel);
    boxxy->Add(mainPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(boxxy);
    this->Layout();

    this->Centre(wxBOTH);

    if(fCanvas->GetFormula().type == COMPLEX_TYPE)
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
    typeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FormulaDialog::OnChoice ), NULL, this );
    acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnAccept), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnApply), NULL, this);
    funcButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FormulaDialog::OnFunc ), NULL, this );
}

FormulaDialog::~FormulaDialog()
{
    typeChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FormulaDialog::OnChoice ), NULL, this );
    acceptButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnAccept), NULL, this);
    applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnApply), NULL, this);
    funcButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FormulaDialog::OnFunc ), NULL, this );
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
        options.type = COMPLEX_TYPE;
    else
        options.type = FIXED_POINT_TYPE;

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
void FormulaDialog::OnFunc( wxCommandEvent& event )
{
    FuncDialog* diag = new FuncDialog(this, wxID_ANY);
    diag->Move(this->GetPosition().x + this->GetSize().x, this->GetPosition().y);
    diag->Show(true);
}
