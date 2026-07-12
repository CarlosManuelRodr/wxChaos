/**
 * @file ScriptSamplePicker.h
 * @brief Defines the dialog used to choose a bundled script sample.
 */

#pragma once

#include <vector>
#include <wx/wx.h>

/**
 * @class ScriptSamplePicker
 * @brief Lets the user select one of the bundled AngelScript samples.
 */
class ScriptSamplePicker final : public wxDialog
{
    wxListBox* _sampleList{};              ///< Displays the available sample scripts.
    wxButton* _selectButton{};             ///< Confirms the selected sample.
    std::vector<wxString> _sampleFiles;    ///< Full paths corresponding to the sample list entries.

    /// @brief Reads the installed ScriptSamples directory and fills the sample list.
    void LoadSamples();
    /// @brief Enables sample selection after the list selection changes.
    /// @param event List-box selection event.
    void OnSelectionChanged(wxCommandEvent& event);
    /// @brief Confirms a sample when it is double-clicked.
    /// @param event List-box activation event.
    void OnSampleActivated(wxCommandEvent& event);
    /// @brief Confirms the current sample selection.
    /// @param event Button click event.
    void OnSelect(wxCommandEvent& event);
    /// @brief Creates a themed header with an icon and title.
    /// @param parent Parent window for the header.
    /// @param text Header title.
    /// @param lightIcon SVG icon used in the light theme.
    /// @param darkIcon SVG icon used in the dark theme.
    /// @return The configured header panel.
    [[nodiscard]] static wxPanel* CreateSectionHeader(wxWindow* parent, const wxString& text,
                                                       const wxString& lightIcon, const wxString& darkIcon);

public:
    /**
     * @brief Creates the sample-selection dialog.
     * @param parent Parent window.
     * @param id Window identifier.
     * @param title Localized dialog title.
     * @param pos Initial dialog position.
     * @param size Initial dialog size.
     * @param style wxWidgets dialog style flags.
     */
    explicit ScriptSamplePicker(wxWindow* parent, wxWindowID id = wxID_ANY,
                                const wxString& title = wxTRANSLATE("Script samples"),
                                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(520, 420),
                                long style = wxDEFAULT_DIALOG_STYLE);
    /// @brief Disconnects dialog event handlers.
    ~ScriptSamplePicker() override;

    /// @brief Gets the full path of the selected bundled sample.
    /// @return The selected sample path, or an empty string when none is selected.
    [[nodiscard]] wxString GetSelectedSample() const;
};
