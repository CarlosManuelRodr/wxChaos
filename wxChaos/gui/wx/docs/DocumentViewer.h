/**
* @file DocumentViewer.h
* @brief Defines a wxWebView-based viewer for bundled documentation.
*/

#pragma once

#include <functional>
#include <vector>
#include <wx/bmpbndl.h>
#include <wx/webview.h>
#include <wx/wx.h>

/**
* @class DocumentViewer
* @brief Frame that displays local HTML documentation through wxWebView.
*
* DocumentViewer replaces the legacy wxHtmlWindow-based help dialog so bundled
* pages can use modern HTML, CSS, and MathJax-rendered formulas.
*/
class DocumentViewer : public wxFrame
{
public:
    /// @brief Callback invoked when a wxchaos:// link is clicked.
    /// @return true when the link was handled by the parent window.
    using WxChaosLinkHandler = std::function<bool(const wxString&)>;

private:
    wxWebView* _webView{};
    wxButton* _backButton{};
    wxButton* _forwardButton{};
    wxButton* _closeButton{};
    wxButton* _openInBrowserButton{};
#ifdef __WXMSW__
    wxButton* _downloadWebViewButton{};
#endif
    wxWindow* _lifetimeOwner{};
    wxString _documentUrl;
    std::vector<wxString> _navigationHistory;
    WxChaosLinkHandler _wxChaosLinkHandler;
    int _navigationHistoryIndex{-1};
    bool _hasLoadedInitialDocument{};
    bool _isNavigatingHistory{};

    /// @brief Creates the themed bitmap used by a navigation button.
    /// @param back True for the back button icon, false for the forward button icon.
    /// @return The themed navigation button bitmap.
    static wxBitmapBundle CreateNavigationButtonBitmap(bool back);

    /// @brief Returns the currently open documentation viewers.
    /// @return Shared list of open documentation viewers.
    static std::vector<DocumentViewer*>& GetOpenViewers();

    /// @brief Adds the WebView2-unavailable explanation and browser actions.
    /// @param mainSizer Frame sizer that receives the fallback panel.
    void CreateWebViewUnavailablePanel(wxBoxSizer* mainSizer);

    /// @brief Adds a normal link navigation to the local document history.
    /// @param url URL that navigation is moving to.
    void AddNavigationHistoryEntry(const wxString& url);

    /// @brief Applies the current wxChaos theme and language to the loaded page.
    void ApplyDocumentPresentation() const;

    /// @brief Enables navigation buttons according to the web view history.
    void UpdateNavigationButtons() const;

    /// @brief Navigates to the previous document history entry.
    /// @param event Button click event.
    void OnBack(wxCommandEvent& event);

    /// @brief Navigates to the next document history entry.
    /// @param event Button click event.
    void OnForward(wxCommandEvent& event);

    /// @brief Updates document chrome when navigation begins.
    /// @param event Web view navigation event.
    void OnNavigating(wxWebViewEvent& event);

    /// @brief Updates document chrome after navigation completes.
    /// @param event Web view navigation event.
    void OnNavigated(wxWebViewEvent& event);

    /// @brief Closes the document when the window that opened it is destroyed.
    /// @param event Destroy event emitted by the lifetime owner.
    void OnOwnerDestroyed(wxWindowDestroyEvent& event);

    /// @brief Applies theme and navigation state after a page finishes loading.
    /// @param event Web view loaded event.
    void OnLoaded(wxWebViewEvent& event);

    /// @brief Opens the requested documentation page in the system browser.
    /// @param event Button click event.
    void OnOpenInBrowser(wxCommandEvent& event);

#ifdef __WXMSW__
    /// @brief Opens the official WebView2 Runtime download page.
    /// @param event Button click event.
    void OnDownloadWebView(wxCommandEvent& event);
#endif

    /// @brief Closes the document viewer.
    /// @param event Button click event.
    void OnClose(wxCommandEvent& event);

public:
    /// @brief Creates a documentation viewer for a local HTML file.
    /// @param htmlFile Local HTML file to load.
    /// @param parent Window that owns the viewer lifetime without forcing native window stacking.
    /// @param id wxWidgets identifier.
    /// @param title Frame title.
    /// @param pos Initial frame position.
    /// @param size Initial frame size.
    /// @param style Frame style.
    /// @param wxChaosLinkHandler
    DocumentViewer(const wxString& htmlFile, wxWindow* parent, wxWindowID id = wxID_ANY,
                   const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxSize(1100, 760), long style = wxDEFAULT_FRAME_STYLE,
                   WxChaosLinkHandler wxChaosLinkHandler = nullptr);
    ~DocumentViewer() override;

    /// @brief Closes every currently open documentation viewer.
    static void CloseOpenViewers();
};
