#include "ScriptNameDialog.h"

#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/frame.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>

#include "AppPaths.h"
#include "common/AppTheme.h"

wxPanel* ScriptNameDialog::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                const wxString& darkIcon)
{
    const auto header = new wxPanel(parent, wxID_ANY);
    header->SetBackgroundColour(AppTheme::ControlBackground());

    const auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    const wxSize iconSize(24, 24);
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    const wxBitmapBundle iconBundle = wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), iconSize);
    const auto iconBitmap = new wxStaticBitmap(header, wxID_ANY, iconBundle.GetBitmap(iconSize));
    iconBitmap->SetBackgroundColour(AppTheme::ControlBackground());
    headerSizer->Add(iconBitmap, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP | wxBOTTOM, 12);

    const auto title = new wxStaticText(header, wxID_ANY, text);
    wxFont titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleFont.SetPointSize(titleFont.GetPointSize() + 1);
    title->SetFont(titleFont);
    title->SetBackgroundColour(AppTheme::ControlBackground());
    title->SetForegroundColour(AppTheme::Foreground());
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

    header->SetSizer(headerSizer);
    header->SetMinSize(wxSize(-1, 52));
    return header;
}

ScriptNameDialog::ScriptNameDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos,
                                   const wxSize& size, const long style)
    : wxDialog(parent, id, wxGetTranslation(title), pos, size, style)
{
    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    const auto mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(CreateSectionHeader(mainPanel, _("Create a new script"), "new_light.svg", "new_dark.svg"),
                    0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto scriptNameSizer = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, _("Set a name for the script")), wxHORIZONTAL);

    const auto scriptNameText = new wxStaticText(scriptNameSizer->GetStaticBox(), wxID_ANY, _("Script name: "));
    scriptNameSizer->Add(scriptNameText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    _scriptNameCtrl = new wxTextCtrl(scriptNameSizer->GetStaticBox(), wxID_ANY, "MyNewScript", wxDefaultPosition, wxDefaultSize, 0);
    scriptNameSizer->Add(_scriptNameCtrl, 1, wxALL, 5);
    panelSizer->Add(scriptNameSizer, 0, wxEXPAND | wxALL, 10);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();
    _okButton = new wxButton(mainPanel, wxID_OK);
    buttonSizer->Add(_okButton, 0, wxALL, 5);
    _cancelButton = new wxButton(mainPanel, wxID_CANCEL);
    buttonSizer->Add(_cancelButton, 0, wxALL, 5);
    panelSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    mainPanel->SetSizer(panelSizer);
    mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 1);
    this->SetSizer(mainSizer);
    const wxSize fittedSize = mainSizer->Fit(this);
    this->wxTopLevelWindowBase::SetMinSize(mainSizer->GetMinSize());
    const int requestedWidth = size.GetWidth() == wxDefaultCoord ? fittedSize.GetWidth() : size.GetWidth();
    this->SetSize(requestedWidth, fittedSize.GetHeight());
    this->wxTopLevelWindowBase::Layout();
    this->Centre(wxBOTH);

    // Connect Events
    _cancelButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _okButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

ScriptNameDialog::~ScriptNameDialog()
{
    // Disconnect Events
    _cancelButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnCancel, this);
    _okButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ScriptNameDialog::OnOk, this);
}

void ScriptNameDialog::OnCancel(wxCommandEvent&)
{
    this->EndModal(wxID_CANCEL);
}
void ScriptNameDialog::OnOk(wxCommandEvent&)
{
    this->EndModal(wxID_OK);
}
wxString ScriptNameDialog::GetScriptName() const
{
    return _scriptNameCtrl->GetValue();
}
