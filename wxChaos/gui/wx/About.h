#pragma once

#include <utility>
#include <vector>

#include <wx/dialog.h>
#include <wx/string.h>

class wxSizer;
class wxWindow;

class AboutDialog final : public wxDialog
{
public:
    explicit AboutDialog(wxWindow* parent);

private:
    using DetailRows = std::vector<std::pair<wxString, wxString>>;

    void CreateControls();
    wxSizer* CreateDetailGrid(wxWindow* parent, const DetailRows& rows) const;

    wxString GetBuildType() const;
    wxString GetArchitecture() const;
    wxString GetCompiler() const;
};
