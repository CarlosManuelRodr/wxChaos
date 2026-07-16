#pragma once

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

/**
 * @class UnrestrictedSpinDoubleCtrl
 * @brief Double-value spin control that allows free-form text entry.
 *
 * The control combines a text box with decrement and increment buttons to create a replacement for wxSpinCtrlDouble
 * that allows free-form text entry without limiting the values by "increment".
 */
class UnrestrictedSpinDoubleCtrl : public wxPanel
{
    wxTextCtrl* _textCtrl{};       ///< Text field containing the editable numeric value.
    wxButton* _decrementButton{};  ///< Button that subtracts the configured increment.
    wxButton* _incrementButton{};  ///< Button that adds the configured increment.
    double _increment{};           ///< Amount added or subtracted when either spin button is clicked.

    /// @brief Handles clicks on the decrement button.
    void OnDecrement(wxCommandEvent& event);

    /// @brief Handles clicks on the increment button.
    void OnIncrement(wxCommandEvent& event);

    /// @brief Adds a delta to the current text value and writes the formatted result back to the text control.
    void AdjustValue(double delta);

    /// @brief Formats a double for display in the text field without unnecessary trailing zeroes.
    static wxString FormatValue(double value);

public:
    /**
     * @brief Creates a free-entry double spin control.
     * @param parent Parent wx window.
     * @param value Initial text value.
     * @param increment Amount used by the decrement and increment buttons.
     */
    UnrestrictedSpinDoubleCtrl(wxWindow* parent, const wxString& value, double increment);

    /// @brief Returns the current text field contents parsed as a double.
    [[nodiscard]] double GetValue() const;

    /// @brief Sets the displayed value using the control's display formatting.
    void SetValue(double value);
};
