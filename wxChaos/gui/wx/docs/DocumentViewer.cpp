#include "docs/DocumentViewer.h"
#include <algorithm>
#include <utility>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/log.h>
#include "AppPaths.h"
#include "common/AppLocalization.h"
#include "common/AppTheme.h"

DocumentViewer::DocumentViewer(const wxString& htmlFile, wxWindow* parent, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style,
                               WxChaosLinkHandler wxChaosLinkHandler, const std::optional<bool> showAtStartup,
                               StartupDisplayChanged startupDisplayChanged)
                               : wxFrame(nullptr, id, title, pos, size, style),
                                 _lifetimeOwner(parent),
                                 _documentUrl(wxFileSystem::FileNameToURL(wxFileName(htmlFile).GetFullPath())),
                                 _wxChaosLinkHandler(std::move(wxChaosLinkHandler)),
                                 _startupDisplayChanged(std::move(startupDisplayChanged))
{
    SetSizeHints(wxSize(900, 620), wxDefaultSize);

    SetIcon(AppPaths::ApplicationIcon());

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
#ifdef __WXMSW__
    if (wxWebView::IsBackendAvailable(wxWebViewBackendEdge))
    {
        _webView = wxWebView::New(this, wxID_ANY, wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize,
                                  wxWebViewBackendEdge);

        const wxVersionInfo backendVersion = wxWebView::GetBackendVersionInfo(wxWebViewBackendEdge);
        wxLogVerbose("Using Microsoft Edge WebView2 %s for documentation",
                     backendVersion.GetNumericVersionString());
    }
#else
    _webView = wxWebView::New(this, wxID_ANY);
#endif

    if (_webView != nullptr)
    {
        mainSizer->Add(_webView, 1, wxALL | wxEXPAND, 5);
    }
    else
    {
#ifdef __WXMSW__
        wxLogWarning("Microsoft Edge WebView2 is unavailable; documentation will open in the system browser");
#else
        wxLogWarning("The platform web view is unavailable; documentation will open in the system browser");
#endif
        CreateWebViewUnavailablePanel(mainSizer);
    }

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    if (_webView != nullptr)
    {
        _backButton = new wxButton(this, wxID_ANY, "", wxDefaultPosition, wxSize(36, 32), 0);
        _forwardButton = new wxButton(this, wxID_ANY, "", wxDefaultPosition, wxSize(36, 32), 0);
        _backButton->SetBitmap(CreateNavigationButtonBitmap(true));
        _forwardButton->SetBitmap(CreateNavigationButtonBitmap(false));
        _backButton->SetToolTip(_("Back"));
        _forwardButton->SetToolTip(_("Forward"));
        buttonSizer->Add(_backButton, 0, wxALL, 5);
        buttonSizer->Add(_forwardButton, 0, wxALL, 5);
    }
    buttonSizer->AddStretchSpacer();
    if (showAtStartup.has_value())
    {
        _showAtStartupCheckBox =
            new wxCheckBox(this, wxID_ANY, _("Show this welcome guide when wxChaos starts"));
        _showAtStartupCheckBox->SetValue(*showAtStartup);
        buttonSizer->Add(_showAtStartupCheckBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    }
    _closeButton = new wxButton(this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND, 5);

    SetSizer(mainSizer);
    wxTopLevelWindowBase::Layout();
    Centre(wxBOTH);

    CloseOpenViewers();
    GetOpenViewers().push_back(this);

    if (_lifetimeOwner != nullptr)
        _lifetimeOwner->Bind(wxEVT_DESTROY, &DocumentViewer::OnOwnerDestroyed, this);

    if (_webView != nullptr)
    {
        _backButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
        _forwardButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
        _webView->Bind(wxEVT_WEBVIEW_NAVIGATING, &DocumentViewer::OnNavigating, this);
        _webView->Bind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
        _webView->Bind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
        _webView->LoadURL(_documentUrl);
    }
    if (_showAtStartupCheckBox != nullptr)
        _showAtStartupCheckBox->Bind(wxEVT_CHECKBOX, &DocumentViewer::OnStartupDisplayChanged, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
    UpdateNavigationButtons();
}

DocumentViewer::~DocumentViewer()
{
    if (_lifetimeOwner != nullptr)
        _lifetimeOwner->Unbind(wxEVT_DESTROY, &DocumentViewer::OnOwnerDestroyed, this);

    if (_webView != nullptr)
    {
        _backButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
        _forwardButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
        _webView->Unbind(wxEVT_WEBVIEW_NAVIGATING, &DocumentViewer::OnNavigating, this);
        _webView->Unbind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
        _webView->Unbind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
    }
    else
    {
        _openInBrowserButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnOpenInBrowser, this);
#ifdef __WXMSW__
        _downloadWebViewButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnDownloadWebView, this);
#endif
    }
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
    if (_showAtStartupCheckBox != nullptr)
        _showAtStartupCheckBox->Unbind(wxEVT_CHECKBOX, &DocumentViewer::OnStartupDisplayChanged, this);

    std::vector<DocumentViewer*>& openViewers = GetOpenViewers();
    openViewers.erase(std::remove(openViewers.begin(), openViewers.end(), this), openViewers.end());
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void DocumentViewer::ApplyDocumentPresentation() const
{
    if (_webView == nullptr)
        return;

    const wxString theme = AppTheme::IsDark() ? "dark" : "light";
    const wxString language = AppLocalization::DocumentationLanguageCode(AppLocalization::CurrentLanguage());
    const wxString script = wxString::Format(
        "document.documentElement.setAttribute('data-theme', '%s');"
        "document.documentElement.setAttribute('data-language', '%s');"
        "document.documentElement.setAttribute('lang', '%s');"
        "document.querySelectorAll('[data-i18n-en],[data-i18n-es]').forEach(function(element) {"
        "var text = element.getAttribute('data-i18n-' + '%s');"
        "if (text !== null) { element.textContent = text; }"
        "});"
        "document.querySelectorAll('[data-i18n-alt-en],[data-i18n-alt-es]').forEach(function(element) {"
        "var text = element.getAttribute('data-i18n-alt-' + '%s');"
        "if (text !== null) { element.setAttribute('alt', text); }"
        "});"
        "document.querySelectorAll('[data-i18n-title-en],[data-i18n-title-es]').forEach(function(element) {"
        "var text = element.getAttribute('data-i18n-title-' + '%s');"
        "if (text !== null) { element.setAttribute('title', text); }"
        "});"
        "if (window.wxChaosInitializeDocument) { window.wxChaosInitializeDocument(); }"
        "if (document.body) { document.body.classList.add('wxchaos-document'); }",
        theme.c_str(),
        language.c_str(),
        language.c_str(),
        language.c_str(),
        language.c_str(),
        language.c_str()
        );
    _webView->RunScript(script);
}

wxBitmapBundle DocumentViewer::CreateNavigationButtonBitmap(const bool back)
{
    const wxString icon = back
        ? (AppTheme::IsDark() ? "arrow_left_dark.svg" : "arrow_left_light.svg")
        : (AppTheme::IsDark() ? "arrow_right_dark.svg" : "arrow_right_light.svg");

    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), wxSize(20, 20));
}

std::vector<DocumentViewer*>& DocumentViewer::GetOpenViewers()
{
    static std::vector<DocumentViewer*> openViewers;
    return openViewers;
}

void DocumentViewer::CreateWebViewUnavailablePanel(wxBoxSizer* mainSizer)
{
    const auto panel = new wxPanel(this);
    const auto panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->AddStretchSpacer();

    const auto title = new wxStaticText(panel, wxID_ANY, _("Documentation viewer unavailable"));
    wxFont titleFont = title->GetFont();
    titleFont.MakeBold();
    titleFont.Scale(1.35);
    title->SetFont(titleFont);
    panelSizer->Add(title, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 12);

#ifdef __WXMSW__
    const wxString explanationText =
        _("The Microsoft Edge WebView2 Runtime is required to display documentation inside wxChaos. "
          "You can open this page in your browser or install WebView2 and try again.");
#else
    const wxString explanationText =
        _("The system web view is unavailable. You can open this documentation page in your browser.");
#endif
    const auto explanation = new wxStaticText(panel, wxID_ANY, explanationText);
    explanation->Wrap(650);
    panelSizer->Add(explanation, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, 24);

    const auto actionSizer = new wxBoxSizer(wxHORIZONTAL);
    _openInBrowserButton = new wxButton(panel, wxID_ANY, _("Open in browser"));
#ifdef __WXMSW__
    _downloadWebViewButton = new wxButton(panel, wxID_ANY, _("Download WebView2"));
    actionSizer->Add(_openInBrowserButton, 0, wxRIGHT, 8);
    actionSizer->Add(_downloadWebViewButton);
#else
    actionSizer->Add(_openInBrowserButton);
#endif
    panelSizer->Add(actionSizer, 0, wxALIGN_CENTER_HORIZONTAL);
    panelSizer->AddStretchSpacer();

    panel->SetSizer(panelSizer);
    mainSizer->Add(panel, 1, wxEXPAND | wxALL, 24);

    _openInBrowserButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnOpenInBrowser, this);
#ifdef __WXMSW__
    _downloadWebViewButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnDownloadWebView, this);
#endif
}

void DocumentViewer::CloseOpenViewers()
{
    const std::vector<DocumentViewer*> openViewers = GetOpenViewers();
    for (DocumentViewer* viewer : openViewers)
    {
        if (viewer != nullptr)
            viewer->Close(true);
    }
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void DocumentViewer::AddNavigationHistoryEntry(const wxString& url)
{
    if (url.IsEmpty())
        return;

    if (_navigationHistoryIndex >= 0 && _navigationHistory[_navigationHistoryIndex] == url)
        return;

    if (_navigationHistoryIndex + 1 < static_cast<int>(_navigationHistory.size()))
        _navigationHistory.erase(_navigationHistory.begin() + _navigationHistoryIndex + 1, _navigationHistory.end());

    _navigationHistory.push_back(url);
    _navigationHistoryIndex = static_cast<int>(_navigationHistory.size()) - 1;
}

void DocumentViewer::UpdateNavigationButtons() const
{
    if (_webView == nullptr)
        return;

    _backButton->Enable(_navigationHistoryIndex > 0);
    _forwardButton->Enable(_navigationHistoryIndex + 1 < static_cast<int>(_navigationHistory.size()));
}

void DocumentViewer::OnBack(wxCommandEvent&)
{
    if (_webView == nullptr || _navigationHistoryIndex <= 0)
        return;

    --_navigationHistoryIndex;
    _isNavigatingHistory = true;
    _webView->LoadURL(_navigationHistory[_navigationHistoryIndex]);
    UpdateNavigationButtons();
}

void DocumentViewer::OnForward(wxCommandEvent&)
{
    if (_webView == nullptr || _navigationHistoryIndex + 1 >= static_cast<int>(_navigationHistory.size()))
        return;

    ++_navigationHistoryIndex;
    _isNavigatingHistory = true;
    _webView->LoadURL(_navigationHistory[_navigationHistoryIndex]);
    UpdateNavigationButtons();
}

void DocumentViewer::OnNavigating(wxWebViewEvent& event)
{
    if (event.GetURL().Lower().StartsWith("wxchaos://"))
    {
        event.Veto();
        if (_wxChaosLinkHandler)
            _wxChaosLinkHandler(event.GetURL());
        return;
    }

    if (_hasLoadedInitialDocument && !_isNavigatingHistory)
        AddNavigationHistoryEntry(event.GetURL());

    UpdateNavigationButtons();
    event.Skip();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void DocumentViewer::OnNavigated(wxWebViewEvent&)
{
    UpdateNavigationButtons();
}

void DocumentViewer::OnOwnerDestroyed(wxWindowDestroyEvent& event)
{
    if (event.GetEventObject() == _lifetimeOwner)
    {
        _lifetimeOwner = nullptr;
        Destroy();
    }

    event.Skip();
}

void DocumentViewer::OnLoaded(wxWebViewEvent&)
{
    if (!_hasLoadedInitialDocument)
        AddNavigationHistoryEntry(_webView->GetCurrentURL());

    _hasLoadedInitialDocument = true;
    _isNavigatingHistory = false;
    ApplyDocumentPresentation();
    UpdateNavigationButtons();
}

void DocumentViewer::OnOpenInBrowser(wxCommandEvent&)
{
    if (!wxLaunchDefaultBrowser(_documentUrl))
        wxLogError("Failed to open documentation in the system browser: %s", _documentUrl);
}

#ifdef __WXMSW__
void DocumentViewer::OnDownloadWebView(wxCommandEvent&)
{
    constexpr auto downloadUrl = "https://developer.microsoft.com/microsoft-edge/webview2/";
    if (!wxLaunchDefaultBrowser(downloadUrl))
        wxLogError("Failed to open the Microsoft Edge WebView2 download page");
}
#endif

void DocumentViewer::OnClose(wxCommandEvent&)
{
    Close(true);
}

void DocumentViewer::OnStartupDisplayChanged(wxCommandEvent&)
{
    if (_startupDisplayChanged)
        _startupDisplayChanged(_showAtStartupCheckBox->GetValue());
}
