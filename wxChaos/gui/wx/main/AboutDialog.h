#pragma once

#include <utility>
#include <vector>

#include <wx/dialog.h>
#include <wx/image.h>
#include <wx/string.h>

class wxSizer;
class wxWindow;
class wxPanel;
class wxPaintEvent;
class wxSizeEvent;

/**
 * @class AboutDialog
 * @brief Dialog for displaying information about the application, the dependencies, and credit icon authors.
 */
class AboutDialog final : public wxDialog
{
public:
    explicit AboutDialog(wxWindow* parent);

private:
    using DetailRows = std::vector<std::pair<wxString, wxString>>;

    void CreateControls();
    wxSizer* CreateDetailGrid(wxWindow* parent, const DetailRows& rows) const;
    void OnBannerPaint(wxPaintEvent& event);
    void OnBannerSize(wxSizeEvent& event);

    static wxString GetBuildType();
    static wxString GetArchitecture();
    static wxString GetCompiler() ;

    wxPanel* _bannerPanel{};
    wxImage _bannerImage;
};
