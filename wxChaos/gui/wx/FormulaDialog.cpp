#include "FormulaDialog.h"
#include "StringFuncs.h"

// FuncDialog
FuncDialog::FuncDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog()
{
    wxXmlResource::Get()->LoadDialog(this, parent, "FuncDialog");

    mainPanel = XRCCTRL(*this, "funcPanel", wxPanel);
    text = XRCCTRL(*this, "text", wxTextCtrl);
    closeButton = XRCCTRL(*this, "closeButton", wxButton);

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
                            : wxDialog()
{
    wxXmlResource::Get()->LoadDialog(this, _parent, "FormulaDialog");

    userDefinedID = _userDefinedID;
    FPuserDefinedID = _FPuserDefinedID;

    parent = _parent;
    this->SetSizeHints(FormulaDialogSize, wxDefaultSize);
    fCanvas = _fCanvas;
    active = Active;
    colorStyle = mColorStyle;

    slider = juliaSlider;
    manual = juliaManual;

    mainPanel = XRCCTRL(*this, "mainPanel", wxPanel);
    formulaCtrl = XRCCTRL(*this, "formulaCtrl", wxTextCtrl);
    bailText = XRCCTRL(*this, "bailText", wxStaticText);
    typeText = XRCCTRL(*this, "typeText", wxStaticText);
    bailCtrl = XRCCTRL(*this, "bailCtrl", wxTextCtrl);
    juliaCheck = XRCCTRL(*this, "juliaCheck", wxCheckBox);
    acceptButton = XRCCTRL(*this, "acceptButton", wxButton);
    applyButton = XRCCTRL(*this, "applyButton", wxButton);
    funcButton = XRCCTRL(*this, "funcButton", wxButton);
    typeChoice = XRCCTRL(*this, "typeChoice", wxChoice);

    formulaCtrl->SetValue(wxString(fCanvas->GetFormula().userFormula));
    bailCtrl->SetValue(num_to_string(fCanvas->GetFormula().bailout));
    if(fCanvas->GetFormula().julia)
        juliaCheck->SetValue(true);
    else
        juliaCheck->SetValue(false);

    if(fCanvas->GetFormula().type == FormulaType::Complex)
    {
        typeChoice->SetSelection(0);
        juliaCheck->Enable(true);
        bailCtrl->Enable(true);
    }
    else
    {
        typeChoice->SetSelection(1);
        juliaCheck->Enable(false);
        bailCtrl->Enable(false);
    }

    this->Centre(wxBOTH);

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FormulaDialog::OnClose));
    typeChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(FormulaDialog::OnChoice), NULL, this);
    acceptButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnAccept), NULL, this);
    applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnApply), NULL, this);
    funcButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FormulaDialog::OnFunc), NULL, this);
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
