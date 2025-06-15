/**
* @file ConstDialog.h
* @brief This header file contains the ConstDialog.
*
* @author Carlos Manuel Rodriguez y Martinez
* @copyright GNU Public License.
* @date 7/18/2012
*/

#ifndef __constdiag
#define __constdiag

#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include "FractalClasses.h"

/**
* @class ConstDialog
* @brief Dialog that sets the constant "K" in a Julia fractal.
*/

class ConstDialog : public wxDialog
{
private:
    wxPanel* dumbPanel;
    wxTextCtrl* realText;
    wxTextCtrl* imText;
    wxButton* okButton;
    wxButton* applyButton;
    Fractal* target;

    double lastReal, lastIm;
    bool* active;

    void OnOk(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

public:
    ConstDialog(bool* Active, Fractal* mTarget, wxWindow* parent, wxWindowID id = wxID_ANY, 
                const wxString& title = wxT("Enter constant"), const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(191, 209), long style = wxDEFAULT_DIALOG_STYLE);
    ~ConstDialog();
};

#endif //__const