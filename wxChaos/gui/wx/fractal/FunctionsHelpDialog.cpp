// ReSharper disable CppDFAMemoryLeak
#include "fractal/FunctionsHelpDialog.h"

#include "AppPaths.h"
#include "common/AppTheme.h"

wxPanel* FunctionsHelpDialog::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
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

wxBitmapBundle FunctionsHelpDialog::CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                     const wxSize& size)
{
    const wxString icon = AppTheme::IsDark() ? darkIcon : lightIcon;
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), size);
}

FunctionsHelpDialog::FunctionsHelpDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
                                         const wxPoint& pos, const wxSize& size, const long style)
                                         : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(460, 250), wxDefaultSize);

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(CreateSectionHeader(_mainPanel, _("Available functions"), "function_light.svg", "function_dark.svg"),
                    0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    _text = new wxTextCtrl(_mainPanel, wxID_ANY,
                           "abs(), sin(), cos(), tan(), sinh(), cosh(),\n"
                           "tanh(), ln(), log(), log10(), exp(), sqrt().",
                           wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_NO_VSCROLL | wxTE_READONLY);
    _text->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL, false, wxEmptyString));
    panelSizer->Add(_text, 1, wxEXPAND | wxALL, 10);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();

    _closeButton = new wxButton(_mainPanel, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL, 5);
    panelSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    _mainPanel->SetSizer(panelSizer);
    _mainPanel->Layout();
    panelSizer->Fit(_mainPanel);
    mainSizer->Add(_mainPanel, 1, wxEXPAND | wxALL, 1);

    this->SetSizer(mainSizer);
    mainSizer->Fit(this);
    this->wxTopLevelWindowBase::SetMinSize(this->GetSize());
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
