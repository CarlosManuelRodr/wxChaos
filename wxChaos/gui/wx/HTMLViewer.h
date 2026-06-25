/**
* @file HTMLViewer.h
* @brief Dialog used to display bundled HTML documentation.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once

#include <wx/wx.h>
#include <wx/html/htmlwin.h>

/**
* @class HTMLViewer
* @brief A simple viewer for HTML files.
*/
class HTMLViewer : public wxDialog
{
    wxHtmlWindow* _htmlView;
    wxButton* _closeButton;

    void OnClose(wxCommandEvent& event);

public:
    HTMLViewer(const wxString& htmlFile, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1100, 760), long style = wxDEFAULT_DIALOG_STYLE);
    ~HTMLViewer() override;
};
