/**
* @file DocumentViewer.h
* @brief Defines a wxWebView-based viewer for bundled documentation.
*/

#pragma once

#include <vector>
#include <wx/bmpbndl.h>
#include <wx/webview.h>
#include <wx/wx.h>

/**
* @class DocumentViewer
* @brief Dialog that displays local HTML documentation through wxWebView.
*
* DocumentViewer replaces the legacy wxHtmlWindow-based help dialog so bundled
* pages can use modern HTML, CSS, and MathJax-rendered formulas.
*/
class DocumentViewer : public wxDialog
{
    wxWebView* _webView{};
    wxButton* _backButton{};
    wxButton* _forwardButton{};
    wxButton* _closeButton{};
    std::vector<wxString> _navigationHistory;
    int _navigationHistoryIndex{-1};
    bool _hasLoadedInitialDocument{};
    bool _isNavigatingHistory{};

    /// @brief Creates the themed bitmap used by a navigation button.
    /// @param back True for the back button icon, false for the forward button icon.
    /// @return The themed navigation button bitmap.
    static wxBitmapBundle CreateNavigationButtonBitmap(bool back);

    /// @brief Adds a normal link navigation to the local document history.
    /// @param url URL that navigation is moving to.
    void AddNavigationHistoryEntry(const wxString& url);

    /// @brief Applies the current wxChaos light or dark theme to the loaded page.
    void ApplyDocumentTheme() const;

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

    /// @brief Applies theme and navigation state after a page finishes loading.
    /// @param event Web view loaded event.
    void OnLoaded(wxWebViewEvent& event);

    /// @brief Closes the document viewer.
    /// @param event Button click event.
    void OnClose(wxCommandEvent& event);

public:
    /// @brief Creates a documentation viewer for a local HTML file.
    /// @param htmlFile Local HTML file to load.
    /// @param parent Parent wxWidgets window.
    /// @param id wxWidgets identifier.
    /// @param title Dialog title.
    /// @param pos Initial dialog position.
    /// @param size Initial dialog size.
    /// @param style Dialog style.
    DocumentViewer(const wxString& htmlFile, wxWindow* parent, wxWindowID id = wxID_ANY,
                   const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxSize(1100, 760), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    ~DocumentViewer() override;
};
