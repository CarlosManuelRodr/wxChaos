#include "scripting/ScriptSamplePicker.h"

#include <algorithm>
#include <wx/dir.h>

#include "AppPaths.h"
#include "common/AppTheme.h"

wxPanel* ScriptSamplePicker::CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
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

ScriptSamplePicker::ScriptSamplePicker(wxWindow* parent, const wxWindowID id, const wxString& title,
                                       const wxPoint& pos, const wxSize& size, const long style)
    : wxDialog(parent, id, wxGetTranslation(title), pos, size, style)
{
    this->SetSizeHints(wxSize(520, 420), wxDefaultSize);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    const auto mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(CreateSectionHeader(mainPanel, _("Select a sample script"), "sample_light.svg", "sample_dark.svg"),
                    0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    const auto samplesSizer = new wxStaticBoxSizer(new wxStaticBox(mainPanel, wxID_ANY, _("Sample scripts")), wxVERTICAL);
    _sampleList = new wxListBox(samplesSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                0, nullptr, wxLB_ALWAYS_SB);
    samplesSizer->Add(_sampleList, 1, wxEXPAND | wxALL, 5);
    panelSizer->Add(samplesSizer, 1, wxEXPAND | wxALL, 10);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();
    _selectButton = new wxButton(mainPanel, wxID_OK, _("Select"));
    _selectButton->Enable(false);
    buttonSizer->Add(_selectButton, 0, wxALL, 5);
    buttonSizer->Add(new wxButton(mainPanel, wxID_CANCEL), 0, wxALL, 5);
    panelSizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    mainPanel->SetSizer(panelSizer);
    mainSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 1);
    this->SetSizer(mainSizer);
    this->LoadSamples();
    this->Layout();
    this->Centre(wxBOTH);

    _sampleList->Bind(wxEVT_LISTBOX, &ScriptSamplePicker::OnSelectionChanged, this);
    _sampleList->Bind(wxEVT_LISTBOX_DCLICK, &ScriptSamplePicker::OnSampleActivated, this);
    _selectButton->Bind(wxEVT_BUTTON, &ScriptSamplePicker::OnSelect, this);
}

ScriptSamplePicker::~ScriptSamplePicker()
{
    _sampleList->Unbind(wxEVT_LISTBOX, &ScriptSamplePicker::OnSelectionChanged, this);
    _sampleList->Unbind(wxEVT_LISTBOX_DCLICK, &ScriptSamplePicker::OnSampleActivated, this);
    _selectButton->Unbind(wxEVT_BUTTON, &ScriptSamplePicker::OnSelect, this);
}

void ScriptSamplePicker::LoadSamples()
{
    const wxDir directory(AppPaths::ScriptSamplesDir());
    if (!directory.IsOpened())
    {
        _sampleList->Append(_("No sample scripts are available."));
        _sampleList->Enable(false);
        return;
    }

    wxString filename;
    bool hasFile = directory.GetFirst(&filename, "*.as", wxDIR_FILES);
    while (hasFile)
    {
        _sampleFiles.push_back(AppPaths::Join(AppPaths::ScriptSamplesDir(), filename));
        hasFile = directory.GetNext(&filename);
    }
    std::sort(_sampleFiles.begin(), _sampleFiles.end());

    for (const wxString& sampleFile : _sampleFiles)
        _sampleList->Append(AppPaths::BaseName(sampleFile));

    if (_sampleFiles.empty())
    {
        _sampleList->Append(_("No sample scripts are available."));
        _sampleList->Enable(false);
    }
}

void ScriptSamplePicker::OnSelectionChanged(wxCommandEvent&)
{
    _selectButton->Enable(_sampleList->GetSelection() != wxNOT_FOUND);
}

void ScriptSamplePicker::OnSampleActivated(wxCommandEvent&)
{
    if (_sampleList->GetSelection() != wxNOT_FOUND)
        this->EndModal(wxID_OK);
}

void ScriptSamplePicker::OnSelect(wxCommandEvent&)
{
    this->EndModal(wxID_OK);
}

wxString ScriptSamplePicker::GetSelectedSample() const
{
    const int selection = _sampleList->GetSelection();
    if (selection == wxNOT_FOUND || selection >= static_cast<int>(_sampleFiles.size()))
        return wxEmptyString;

    return _sampleFiles[selection];
}
