#pragma once
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
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
    wxTextCtrl* _scriptNameCtrl;    ///< Input field for the new script name.
    wxButton* _okButton;            ///< Confirms the script name.
    wxButton* _cancelButton;        ///< Closes the dialog without creating a script.

    /// @brief Creates a themed header with an icon and title.
    /// @param parent Parent window for the header.
    /// @param text Header title.
    /// @param lightIcon SVG icon used in the light theme.
    /// @param darkIcon SVG icon used in the dark theme.
    /// @return The configured header panel.
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text,
                                                       const wxString& lightIcon, const wxString& darkIcon);
    /// @brief Cancels script creation.
    /// @param event Button click event.
    void OnCancel(wxCommandEvent& event);
    /// @brief Confirms the entered script name.
    /// @param event Button click event.
    void OnOk(wxCommandEvent& event);
public:
    /**
     * @brief Creates the script-name dialog.
     * @param parent Parent window.
     * @param id Window identifier.
     * @param title Localized dialog title.
     * @param pos Initial dialog position.
     * @param size Initial dialog size.
     * @param style wxWidgets dialog style flags.
     */
    explicit ScriptNameDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 220), long style = wxDEFAULT_DIALOG_STYLE);
    /// @brief Disconnects dialog event handlers.
    ~ScriptNameDialog() override;
    /// @brief Gets the script name entered by the user.
    /// @return The current script-name field value.
    [[nodiscard]] wxString GetScriptName() const;
};
