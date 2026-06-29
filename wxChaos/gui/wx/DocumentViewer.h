/**
* @file DocumentViewer.h
* @brief Defines a wxWebView-based viewer for bundled documentation.
*/

#pragma once

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
