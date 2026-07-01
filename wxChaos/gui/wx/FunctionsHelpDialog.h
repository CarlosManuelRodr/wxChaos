/**
* @file FunctionsHelpDialog.h
* @brief Defines a dialog that shows functions available to user formulas.
*
* @copyright GNU Public License.
* @author Carlos Manuel Rodriguez y Martinez
* @date 7/19/2012
*/

#pragma once

#include <wx/wx.h>
#include <wx/bmpbndl.h>

/**
* @class FunctionsHelpDialog
* @brief Dialog that shows the available functions.
*/
class FunctionsHelpDialog : public wxDialog
{
    wxPanel* _mainPanel;
    wxTextCtrl* _text;
    wxButton* _closeButton;

    void OnClose(wxCommandEvent& event);
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text, const wxString& lightIcon,
                                                      const wxString& darkIcon);
    [[nodiscard]] static wxBitmapBundle CreateIconBundle(const wxString& lightIcon, const wxString& darkIcon,
                                                         const wxSize& size);

public:
    explicit FunctionsHelpDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(460, 250),
                                 long style = wxDEFAULT_DIALOG_STYLE);
    ~FunctionsHelpDialog() override;
};
