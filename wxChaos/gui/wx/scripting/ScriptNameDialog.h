#pragma once
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/statbmp.h>
#include <wx/dialog.h>

/**
* @class ScriptNameDialog
* @brief Prompts for the name of a new user script.
*/
class ScriptNameDialog : public wxDialog
{
    wxStaticText* _scriptNameText;
    wxTextCtrl* _scriptNameCtrl;
    wxStdDialogButtonSizer* _buttonsSizer;
    wxButton* _buttonsSizerOk;
    wxButton* _buttonsSizerCancel;

    void OnCancel(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
public:
    explicit ScriptNameDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 220), long style = wxDEFAULT_DIALOG_STYLE);
    ~ScriptNameDialog() override;
    [[nodiscard]] wxString GetScriptName() const;
};