#pragma once

#include <functional>
#include <vector>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include "UnrestrictedSpinDoubleCtrl.h"

class Fractal;
class PanelOptions;

/**
 * @class FractalOptionsPanel
 * @brief Builds and applies editable UI controls for a fractal's PanelOptions.
 *
 * The panel owns the dynamic wx controls created from the current target fractal. It can optionally expose Julia
 * constants, then creates additional controls from the target's PanelOptions metadata and writes their edited values
 * back to the linked fractal variables when Apply() is called.
 */
class FractalOptionsPanel : public wxPanel
{
    wxBoxSizer* _sizer{};                ///< Main vertical layout for the header and dynamic controls.
    wxStaticText* _kRealLabel{};         ///< Label for the Julia real constant field.
    wxTextCtrl* _kRealCtrl{};            ///< Text field for the Julia real constant.
    wxStaticText* _kImaginaryLabel{};    ///< Label for the Julia imaginary constant field.
    wxTextCtrl* _kImaginaryCtrl{};       ///< Text field for the Julia imaginary constant.
    wxButton* _applyButton{};            ///< Button that applies the current control values to the target fractal.
    Fractal* _target{};                  ///< Fractal whose option metadata and linked values are being edited.
    bool _showJuliaConstants{};          ///< Whether Julia constant controls should be shown above panel options.

    std::vector<int> _foundLabels;              ///< PanelOptions indices that were rendered as static labels.
    std::vector<int> _foundTextControls;        ///< Double-target indices represented by text controls.
    std::vector<int> _foundSpinControls;        ///< Integer-target indices represented by spin controls.
    std::vector<int> _foundSpinDoubleControls;  ///< Double-target indices represented by double spin controls.
    std::vector<int> _foundCheckBoxes;          ///< Boolean-target indices represented by check boxes.
    std::function<void()> _applyHandler;        ///< Optional callback invoked after Apply() handles the target.
    std::vector<wxCheckBox*> _checkBoxes;       ///< Check boxes created from boolean-linked options.
    std::vector<wxWindow*> _dynamicControls;    ///< All controls created for the current target and destroyed on rebuild.
    std::vector<wxTextCtrl*> _textControls;     ///< Text controls created from double-linked text options.
    std::vector<wxSpinCtrl*> _spinControls;     ///< Spin controls created from integer-linked spin options.
    std::vector<UnrestrictedSpinDoubleCtrl*> _spinDoubleControls;  ///< Spin controls created from double-linked options.

    /// @brief Creates the themed header shown above the dynamic option controls.
    wxPanel* CreateFractalOptionsHeader();

    /// @brief Rebuilds dynamic controls from the current target fractal.
    void Build();

    /// @brief Destroys all controls owned by the current dynamic build.
    void ClearDynamicControls();

    /// @brief Returns the localized display label for a target option.
    wxString TranslateOptionLabel(PanelOptions* panelOptions, unsigned int index) const;

    /// @brief Handles the Apply button click.
    void OnApply(wxCommandEvent& event);

public:
    /**
     * @brief Creates the fractal options panel.
     * @param parent Parent wx window.
     * @param showJuliaConstants Whether to include Julia constant controls when the target supports them.
     */
    explicit FractalOptionsPanel(wxWindow* parent, bool showJuliaConstants = false);

    /// @brief Unbinds handlers owned by the panel.
    ~FractalOptionsPanel() override;

    /// @brief Sets the fractal edited by this panel and rebuilds the dynamic controls.
    void SetTarget(Fractal* target);

    /// @brief Clears the current target and removes all target-specific controls.
    void ClearTarget();

    /// @brief Writes current control values back to the target fractal.
    void Apply() const;

    /// @brief Sets a callback invoked after the Apply button applies panel values.
    void SetApplyHandler(std::function<void()> handler);

    /// @brief Returns whether this panel has target options that should be shown.
    [[nodiscard]] bool HasVisibleOptions() const;
};
