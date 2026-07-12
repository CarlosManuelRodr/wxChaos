#include "ScriptNameDialog.h"
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/frame.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/button.h>

ScriptNameDialog::ScriptNameDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                                   const wxSize& size, const long style) : wxDialog(parent, id, wxGetTranslation(title), pos, size, style)
{
    this->SetSizeHints(wxSize(600, 220), wxDefaultSize);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);

    const auto scriptNameSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Set a name for the script")), wxHORIZONTAL);

    _scriptNameText = new wxStaticText(scriptNameSizer->GetStaticBox(), wxID_ANY, _("Script name: "), wxDefaultPosition, wxDefaultSize, 0);
    _scriptNameText->Wrap(-1);
    scriptNameSizer->Add(_scriptNameText, 0, wxALL, 5);

    _scriptNameCtrl = new wxTextCtrl(scriptNameSizer->GetStaticBox(), wxID_ANY, "MyNewScript", wxDefaultPosition, wxDefaultSize, 0);
    scriptNameSizer->Add(_scriptNameCtrl, 1, wxALL, 5);

    mainSizer->Add(scriptNameSizer, 0, wxEXPAND, 5);

    _buttonsSizer = new wxStdDialogButtonSizer();
    _buttonsSizerOk = new wxButton(this, wxID_OK);
    _buttonsSizer->AddButton(_buttonsSizerOk);
    _buttonsSizerCancel = new wxButton(this, wxID_CANCEL);
    _buttonsSizer->AddButton(_buttonsSizerCancel);
    _buttonsSizer->Realize();

    mainSizer->Add(_buttonsSizer, 0, wxEXPAND, 5);


    this->SetSizer(mainSizer);
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    // Connect Events
    _buttonsSizerCancel->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _buttonsSizerOk->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

ScriptNameDialog::~ScriptNameDialog()
{
    // Disconnect Events
    _buttonsSizerCancel->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _buttonsSizerOk->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

void ScriptNameDialog::OnCancel(wxCommandEvent&)
{
    this->EndModal(0);
}
void ScriptNameDialog::OnOk(wxCommandEvent&)
{
    this->EndModal(1);
}
wxString ScriptNameDialog::GetScriptName() const
{
    return _scriptNameCtrl->GetValue();
}