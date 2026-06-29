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
    wxButton* _closeButton{};

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
