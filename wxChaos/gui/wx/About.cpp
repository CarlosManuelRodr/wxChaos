#include "About.h"

#include <SFML/Config.hpp>
#include <angelscript.h>
#include <mpDefines.h>

#include <wx/bmpbndl.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/version.h>

#include "AppPaths.h"
#include "global.h"

AboutDialog::AboutDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("About wxChaos"), wxDefaultPosition, wxDefaultSize,
               wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU)
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetBackgroundColour(wxColour(244, 247, 251));
    CreateControls();
    GetSizer()->Fit(this);
    SetMinSize(GetSize());
    CentreOnParent();
}

void AboutDialog::CreateControls()
{
    const wxColour ink(25, 37, 55);
    const wxColour muted(92, 105, 123);
    const wxColour accent(16, 116, 157);
    const int outerMargin = FromDIP(20);
    const int sectionGap = FromDIP(14);

    auto* root = new wxBoxSizer(wxVERTICAL);
    SetSizer(root);

    const wxSize bannerSize = FromDIP(wxSize(680, 170));
    const wxBitmapBundle banner = wxBitmapBundle::FromSVGFile(
        AppPaths::ResourceFile({wxT("wxChaosAbout.svg")}), bannerSize);
    auto* header = new wxStaticBitmap(this, wxID_ANY, banner, wxDefaultPosition, bannerSize);
    root->Add(header, 0, wxEXPAND);

    auto* content = new wxPanel(this, wxID_ANY);
    content->SetBackgroundColour(wxColour(244, 247, 251));
    auto* contentSizer = new wxBoxSizer(wxVERTICAL);
    content->SetSizer(contentSizer);
    root->Add(content, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, outerMargin);

    auto* intro = new wxStaticText(
        content,
        wxID_ANY,
        _("An open-source playground for exploring fractals, dynamical systems, and generative mathematics."));
    intro->SetForegroundColour(ink);
    intro->SetFont(wxFontInfo(GetFont().GetPointSize() + 2).Bold());
    intro->Wrap(bannerSize.GetWidth() - 2 * outerMargin);
    contentSizer->Add(intro, 0, wxEXPAND | wxTOP, sectionGap);

    auto* byline = new wxStaticText(
        content,
        wxID_ANY,
        _("Created by Carlos Manuel Rodriguez y Martinez. Originally begun in 2012 and modernized with wxWidgets 3 and SFML 2."));
    byline->SetForegroundColour(muted);
    byline->Wrap(bannerSize.GetWidth() - 2 * outerMargin);
    contentSizer->Add(byline, 0, wxEXPAND | wxTOP, FromDIP(6));

    auto* links = new wxBoxSizer(wxHORIZONTAL);
    auto* sourceLink = new wxHyperlinkCtrl(
        content,
        wxID_ANY,
        _("Source code"),
        wxT("https://github.com/CarlosManuelRodr/wxChaos"));
    auto* emailLink = new wxHyperlinkCtrl(
        content,
        wxID_ANY,
        _("Contact the author"),
        wxT("mailto:fis.carlosmanuel@gmail.com"));
    sourceLink->SetNormalColour(accent);
    emailLink->SetNormalColour(accent);
    links->Add(sourceLink);
    links->Add(FromDIP(18), 0);
    links->Add(emailLink);
    contentSizer->Add(links, 0, wxTOP, FromDIP(8));

    contentSizer->Add(new wxStaticLine(content), 0, wxEXPAND | wxTOP | wxBOTTOM, sectionGap);

    auto* details = new wxBoxSizer(wxHORIZONTAL);
    contentSizer->Add(details, 0, wxEXPAND);

    auto addSection = [&](const wxString& title, const DetailRows& rows)
    {
        auto* section = new wxBoxSizer(wxVERTICAL);
        auto* heading = new wxStaticText(content, wxID_ANY, title);
        heading->SetForegroundColour(accent);
        heading->SetFont(wxFontInfo(GetFont().GetPointSize() + 1).Bold());
        section->Add(heading, 0, wxBOTTOM, FromDIP(8));
        section->Add(CreateDetailGrid(content, rows), 0, wxEXPAND);
        return section;
    };

    details->Add(
        addSection(
            _("Application"),
            {
                {_("Version"), wxString::FromUTF8(APP_VERSION)},
                {_("Build"), wxString::Format(wxT("%s (%s)"), GetBuildType(), GetArchitecture())},
                {_("Compiler"), GetCompiler()},
            }),
        1,
        wxEXPAND | wxRIGHT,
        FromDIP(20));

    details->Add(
        addSection(
            _("Built with"),
            {
                {wxT("wxWidgets"), wxVERSION_STRING},
                {wxT("SFML"), wxString::Format(wxT("%d.%d.%d"), SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH)},
                {wxT("AngelScript"), wxString::FromUTF8(ANGELSCRIPT_VERSION_STRING)},
                {wxT("muParserX"), wxString(MUP_PARSER_VERSION)},
            }),
        1,
        wxEXPAND | wxLEFT,
        FromDIP(20));

    auto* footer = new wxBoxSizer(wxHORIZONTAL);
    auto* license = new wxStaticText(content, wxID_ANY, _("Licensed under GPLv3"));
    license->SetForegroundColour(muted);
    footer->Add(license, 1, wxALIGN_CENTER_VERTICAL);
    footer->Add(new wxButton(content, wxID_OK, _("Close")), 0, wxLEFT, FromDIP(20));
    contentSizer->Add(footer, 0, wxEXPAND | wxTOP, sectionGap);
}

wxSizer* AboutDialog::CreateDetailGrid(wxWindow* parent, const DetailRows& rows) const
{
    const wxColour labelColour(92, 105, 123);
    const wxColour valueColour(25, 37, 55);
    auto* grid = new wxFlexGridSizer(2, FromDIP(6), FromDIP(14));
    grid->AddGrowableCol(1);

    for (const auto& [label, value] : rows)
    {
        auto* labelText = new wxStaticText(parent, wxID_ANY, label);
        labelText->SetForegroundColour(labelColour);
        labelText->SetFont(wxFontInfo(GetFont().GetPointSize()).Bold());
        grid->Add(labelText, 0, wxALIGN_TOP);

        auto* valueText = new wxStaticText(parent, wxID_ANY, value);
        valueText->SetForegroundColour(valueColour);
        grid->Add(valueText, 1, wxEXPAND);
    }

    return grid;
}

wxString AboutDialog::GetBuildType() const
{
#ifdef WXCHAOS_BUILD_TYPE
    return wxString::FromUTF8(WXCHAOS_BUILD_TYPE);
#elif defined(NDEBUG)
    return wxT("Release");
#else
    return wxT("Debug");
#endif
}

wxString AboutDialog::GetArchitecture() const
{
#if defined(_M_X64) || defined(__x86_64__)
    return wxT("x64");
#elif defined(_M_IX86) || defined(__i386__)
    return wxT("x86");
#elif defined(_M_ARM64) || defined(__aarch64__)
    return wxT("ARM64");
#else
    return wxT("unknown architecture");
#endif
}

wxString AboutDialog::GetCompiler() const
{
#if defined(_MSC_VER)
    return wxString::Format(wxT("MSVC %d.%02d"), _MSC_VER / 100, _MSC_VER % 100);
#elif defined(__clang__)
    return wxString::Format(wxT("Clang %d.%d.%d"), __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    return wxString::Format(wxT("GCC %d.%d.%d"), __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
    return wxT("Unknown compiler");
#endif
}
