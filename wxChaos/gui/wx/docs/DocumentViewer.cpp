#include "docs/DocumentViewer.h"
#include <algorithm>
#include <utility>
#include <wx/filename.h>
#include <wx/filesys.h>
#include "AppPaths.h"
#include "common/AppLocalization.h"
#include "common/AppTheme.h"

DocumentViewer::DocumentViewer(const wxString& htmlFile, wxWindow* parent, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style,
                               WxChaosLinkHandler wxChaosLinkHandler)
                               : wxFrame(nullptr, id, title, pos, size, style),
                                 _lifetimeOwner(parent),
                                 _wxChaosLinkHandler(std::move(wxChaosLinkHandler))
{
    SetSizeHints(wxSize(900, 620), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    _webView = wxWebView::New(this, wxID_ANY);
    mainSizer->Add(_webView, 1, wxALL | wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    _backButton = new wxButton(this, wxID_ANY, "", wxDefaultPosition, wxSize(36, 32), 0);
    _forwardButton = new wxButton(this, wxID_ANY, "", wxDefaultPosition, wxSize(36, 32), 0);
    _backButton->SetBitmap(CreateNavigationButtonBitmap(true));
    _forwardButton->SetBitmap(CreateNavigationButtonBitmap(false));
    _backButton->SetToolTip(_("Back"));
    _forwardButton->SetToolTip(_("Forward"));
    buttonSizer->Add(_backButton, 0, wxALL, 5);
    buttonSizer->Add(_forwardButton, 0, wxALL, 5);
    buttonSizer->AddStretchSpacer();
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

    _backButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
    _forwardButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
    _webView->Bind(wxEVT_WEBVIEW_NAVIGATING, &DocumentViewer::OnNavigating, this);
    _webView->Bind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
    _webView->Bind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
    _webView->LoadURL(wxFileSystem::FileNameToURL(wxFileName(htmlFile).GetFullPath()));
    UpdateNavigationButtons();
}

DocumentViewer::~DocumentViewer()
{
    if (_lifetimeOwner != nullptr)
        _lifetimeOwner->Unbind(wxEVT_DESTROY, &DocumentViewer::OnOwnerDestroyed, this);

    _backButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
    _forwardButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
    _webView->Unbind(wxEVT_WEBVIEW_NAVIGATING, &DocumentViewer::OnNavigating, this);
    _webView->Unbind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
    _webView->Unbind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);

    std::vector<DocumentViewer*>& openViewers = GetOpenViewers();
    openViewers.erase(std::remove(openViewers.begin(), openViewers.end(), this), openViewers.end());
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void DocumentViewer::ApplyDocumentPresentation() const
{
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
    _backButton->Enable(_navigationHistoryIndex > 0);
    _forwardButton->Enable(_navigationHistoryIndex + 1 < static_cast<int>(_navigationHistory.size()));
}

void DocumentViewer::OnBack(wxCommandEvent&)
{
    if (_navigationHistoryIndex <= 0)
        return;

    --_navigationHistoryIndex;
    _isNavigatingHistory = true;
    _webView->LoadURL(_navigationHistory[_navigationHistoryIndex]);
    UpdateNavigationButtons();
}

void DocumentViewer::OnForward(wxCommandEvent&)
{
    if (_navigationHistoryIndex + 1 >= static_cast<int>(_navigationHistory.size()))
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

void DocumentViewer::OnClose(wxCommandEvent&)
{
    Close(true);
}
