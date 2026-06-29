#include "DocumentViewer.h"
#include <wx/filename.h>
#include <wx/filesys.h>
#include "AppPaths.h"
#include "AppTheme.h"

DocumentViewer::DocumentViewer(const wxString& htmlFile, wxWindow* parent, const wxWindowID id,
                               const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
                               : wxDialog(parent, id, title, pos, size, style)
{
    SetSizeHints(wxSize(900, 620), wxDefaultSize);

    const wxIcon icon(AppPaths::ResourceFile({"Icons", "icon.png"}), wxBITMAP_TYPE_PNG);
    SetIcon(icon);

    const auto mainSizer = new wxBoxSizer(wxVERTICAL);
    _webView = wxWebView::New(this, wxID_ANY);
    mainSizer->Add(_webView, 1, wxALL | wxEXPAND, 5);

    const auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    _backButton = new wxButton(this, wxID_ANY, "Back", wxDefaultPosition, wxDefaultSize, 0);
    _forwardButton = new wxButton(this, wxID_ANY, "Forward", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_backButton, 0, wxALL, 5);
    buttonSizer->Add(_forwardButton, 0, wxALL, 5);
    buttonSizer->AddStretchSpacer();
    _closeButton = new wxButton(this, wxID_ANY, "Close", wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(_closeButton, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND, 5);

    SetSizer(mainSizer);
    Layout();
    Centre(wxBOTH);

    _backButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
    _forwardButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
    _webView->Bind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
    _webView->Bind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
    _closeButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
    _webView->LoadURL(wxFileSystem::FileNameToURL(wxFileName(htmlFile).GetFullPath()));
    UpdateNavigationButtons();
}

DocumentViewer::~DocumentViewer()
{
    _backButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnBack, this);
    _forwardButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnForward, this);
    _webView->Unbind(wxEVT_WEBVIEW_NAVIGATED, &DocumentViewer::OnNavigated, this);
    _webView->Unbind(wxEVT_WEBVIEW_LOADED, &DocumentViewer::OnLoaded, this);
    _closeButton->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &DocumentViewer::OnClose, this);
}

void DocumentViewer::ApplyDocumentTheme() const
{
    const wxString theme = AppTheme::IsDark() ? "dark" : "light";
    const wxString script = wxString::Format(
        "document.documentElement.setAttribute('data-theme', '%s');"
        "if (document.body) { document.body.classList.add('wxchaos-document'); }",
        theme.c_str());
    _webView->RunScript(script);
}

void DocumentViewer::UpdateNavigationButtons() const
{
    _backButton->Enable(_webView->CanGoBack());
    _forwardButton->Enable(_webView->CanGoForward());
}

void DocumentViewer::OnBack(wxCommandEvent&)
{
    if (_webView->CanGoBack())
        _webView->GoBack();
}

void DocumentViewer::OnForward(wxCommandEvent&)
{
    if (_webView->CanGoForward())
        _webView->GoForward();
}

void DocumentViewer::OnNavigated(wxWebViewEvent&)
{
    UpdateNavigationButtons();
}

void DocumentViewer::OnLoaded(wxWebViewEvent&)
{
    ApplyDocumentTheme();
    UpdateNavigationButtons();
}

void DocumentViewer::OnClose(wxCommandEvent&)
{
    Close(true);
}
