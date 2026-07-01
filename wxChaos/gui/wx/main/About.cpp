#include "main/About.h"

#include <SFML/Config.hpp>
#include <angelscript.h>
#include <mpDefines.h>
#include <mpfr.h>
#include <symengine/symengine_config.h>

#include <wx/button.h>
#include <wx/dcbuffer.h>
#include <wx/hyperlink.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/version.h>

#include "AppPaths.h"
#include "global.h"

AboutDialog::AboutDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("About wxChaos"), wxDefaultPosition, wxDefaultSize,
               wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU)
{
    wxWindow::SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxWindowBase::SetBackgroundColour(wxColour(244, 247, 251));
    CreateControls();
    GetSizer()->Fit(this);
    SetClientSize(640, GetClientSize().GetHeight());
    wxTopLevelWindowBase::Layout();
    wxTopLevelWindowBase::SetMinSize(GetSize());
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

    constexpr int bannerWidth = 640;
    constexpr int bannerHeight = 160;
    const wxSize bannerSize(bannerWidth, bannerHeight);
    _bannerImage.LoadFile(
        AppPaths::ResourceFile({"wxChaosAbout.png"}),
        wxBITMAP_TYPE_PNG);
    wxASSERT_MSG(_bannerImage.IsOk(), _("Could not load the About banner"));

    _bannerPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, bannerSize);
    _bannerPanel->SetMinSize(bannerSize);
    _bannerPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    _bannerPanel->Bind(wxEVT_PAINT, &AboutDialog::OnBannerPaint, this);
    _bannerPanel->Bind(wxEVT_SIZE, &AboutDialog::OnBannerSize, this);
    root->Add(_bannerPanel, 0, wxEXPAND);

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
        _("Created by Carlos Manuel Rodriguez Martinez."));
    byline->SetForegroundColour(muted);
    byline->Wrap(bannerSize.GetWidth() - 2 * outerMargin);
    contentSizer->Add(byline, 0, wxEXPAND | wxTOP, FromDIP(6));

    auto* links = new wxBoxSizer(wxHORIZONTAL);
    auto* sourceLink = new wxHyperlinkCtrl(
        content,
        wxID_ANY,
        _("Source code"),
        "https://github.com/CarlosManuelRodr/wxChaos");
    auto* emailLink = new wxHyperlinkCtrl(
        content,
        wxID_ANY,
        _("Contact the author"),
        "mailto:fis.carlosmanuel@gmail.com");
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
                {_("Build"), wxString::Format("%s (%s)", GetBuildType(), GetArchitecture())},
                {_("Compiler"), GetCompiler()},
            }),
        1,
        wxEXPAND | wxRIGHT,
        FromDIP(20));

    details->Add(
        addSection(
            _("Built with"),
            {
                {"wxWidgets", wxVERSION_STRING},
                {"SFML", wxString::Format("%d.%d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH)},
                {"AngelScript", wxString::FromUTF8(ANGELSCRIPT_VERSION_STRING)},
                {"muParserX", wxString(MUP_PARSER_VERSION)},
                {"MPFR", wxString::FromUTF8(MPFR_VERSION_STRING)},
                {"SymEngine", wxString::FromUTF8(SYMENGINE_VERSION)},
            }),
        1,
        wxEXPAND | wxLEFT,
        FromDIP(20));

    auto* iconsRow = new wxBoxSizer(wxHORIZONTAL);
    auto* iconsLabel = new wxStaticText(content, wxID_ANY, _("Icons by Streamline \u2014 "));
    iconsLabel->SetForegroundColour(muted);
    auto* iconsLink = new wxHyperlinkCtrl(
        content,
        wxID_ANY,
        "https://www.streamlinehq.com/",
        "https://www.streamlinehq.com/");
    iconsLink->SetNormalColour(accent);
    iconsRow->Add(iconsLabel, 0, wxALIGN_CENTER_VERTICAL);
    iconsRow->Add(iconsLink, 0, wxALIGN_CENTER_VERTICAL);
    contentSizer->Add(iconsRow, 0, wxTOP, FromDIP(4));

    auto* footer = new wxBoxSizer(wxHORIZONTAL);
    auto* license = new wxStaticText(content, wxID_ANY, _("Licensed under GPLv3"));
    license->SetForegroundColour(muted);
    footer->Add(license, 1, wxALIGN_CENTER_VERTICAL);
    footer->Add(new wxButton(content, wxID_OK, _("Close")), 0, wxLEFT, FromDIP(20));
    contentSizer->Add(footer, 0, wxEXPAND | wxTOP, sectionGap);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AboutDialog::OnBannerPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(_bannerPanel);
    const wxSize size = _bannerPanel->GetClientSize();
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();

    if (!_bannerImage.IsOk() || size.GetWidth() <= 0 || size.GetHeight() <= 0)
        return;

    const double imageRatio = static_cast<double>(_bannerImage.GetWidth()) / _bannerImage.GetHeight();
    int drawWidth = size.GetWidth();
    int drawHeight = static_cast<int>(drawWidth / imageRatio);
    if (drawHeight > size.GetHeight())
    {
        drawHeight = size.GetHeight();
        drawWidth = static_cast<int>(drawHeight * imageRatio);
    }

    const int x = (size.GetWidth() - drawWidth) / 2;
    const int y = (size.GetHeight() - drawHeight) / 2;
    const wxImage scaled = _bannerImage.Scale(
        drawWidth,
        drawHeight,
        wxIMAGE_QUALITY_HIGH);
    dc.DrawBitmap(wxBitmap(scaled), x, y, false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AboutDialog::OnBannerSize(wxSizeEvent& event)
{
    _bannerPanel->Refresh();
    event.Skip();
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

wxString AboutDialog::GetBuildType()
{
#ifdef WXCHAOS_BUILD_TYPE
    return wxString::FromUTF8(WXCHAOS_BUILD_TYPE);
#elif defined(NDEBUG)
    return "Release";
#else
    return "Debug";
#endif
}

wxString AboutDialog::GetArchitecture()
{
#if defined(_M_X64) || defined(__x86_64__)
    return "x64";
#elif defined(_M_IX86) || defined(__i386__)
    return "x86";
#elif defined(_M_ARM64) || defined(__aarch64__)
    return "ARM64";
#else
    return "unknown architecture";
#endif
}

wxString AboutDialog::GetCompiler()
{
#if defined(_MSC_VER)
    return wxString::Format("MSVC %d.%02d", _MSC_VER / 100, _MSC_VER % 100);
#elif defined(__clang__)
    return wxString::Format("Clang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    return wxString::Format("GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
    return "Unknown compiler";
#endif
}
