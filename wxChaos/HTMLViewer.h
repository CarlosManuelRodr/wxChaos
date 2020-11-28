/**
* @file HTMLViewer.h
* @brief All the fractal drawing related classes.
*
* For the definition of how each fractal is rendered look into fractalTypes.h
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 9/22/2012
*/

#pragma once
#ifndef _htmlViewer
#define _htmlViewer

#include <wx/wx.h>
#include <wx/html/htmlwin.h>

/**
* @class HTMLViewer
* @brief A simple viewer for HTML files.
*/
class HTMLViewer : public wxDialog
{
    wxHtmlWindow* htmlView;
    wxButton* CloseButton;

    void OnClose( wxCommandEvent& event );

public:
    HTMLViewer( wxString htmlFile, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize( 849,591 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~HTMLViewer();
};

#endif
