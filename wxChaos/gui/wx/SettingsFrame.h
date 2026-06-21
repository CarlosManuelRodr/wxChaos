#pragma once

#include <functional>
#include <vector>
#include <wx/listbook.h>
#include <wx/wx.h>
#include "config/AppConfigStore.h"
#include "wxGradient.h"

/// @brief Emitted after the settings frame has been closed.
wxDECLARE_EVENT(wxEVT_SETTINGS_FRAME_CLOSED, wxCommandEvent);

/**
 * @class SettingsFrame
 * @brief Provides a graphical editor for values persisted by AppConfigStore.
 *
 * Settings are grouped into General, Presets, and Rendering pages. Applying
 * changes saves the configuration and notifies the owning window.
 */
class SettingsFrame final : public wxFrame
{
public:
    /**
     * @brief Creates the application settings frame.
     * @param parent Window that owns the frame and receives the close event.
     * @param config Configuration values used to initialize the controls.
     * @param configChanged Callback invoked after configuration is saved.
     */
    SettingsFrame(wxWindow* parent, const AppConfig& config, std::function<void(const AppConfig&)> configChanged);

private:
    /// @brief Creates the page containing startup behavior settings.
    /// @return Newly created General settings page.
    wxPanel* CreateGeneralPage();

    /// @brief Creates the page containing default fractal settings.
    /// @return Newly created Presets settings page.
    wxPanel* CreatePresetsPage();

    /// @brief Creates the page containing default coloring settings.
    /// @return Newly created Rendering settings page.
    wxPanel* CreateRenderingPage();

    /// @brief Populates every control from the supplied configuration.
    /// @param config Configuration values to display.
    void LoadControls(const AppConfig& config);

    /// @brief Builds a configuration object from the current control values.
    /// @return Configuration represented by the settings controls.
    AppConfig ReadControls();

    /// @brief Redraws the bitmap that previews the selected gradient.
    void UpdateGradientPreview() const;

    /// @brief Updates the selected gradient from a built-in color style.
    /// @param style Color style used to replace the current gradient.
    void ApplyColorStyle(ColorPaletteTypes style);

    /// @brief Persists and applies the current control values.
    /// @param closeAfterSave Whether to close the frame after saving.
    void SaveSettings(bool closeAfterSave);

    /// @brief Changes the default gradient when a color style is selected.
    /// @param event Choice event that selected the color style.
    void OnColorStyleChanged(wxCommandEvent& event);

    /// @brief Opens the visual gradient editor.
    /// @param event Button event that requested the editor.
    void OnEditGradient(wxCommandEvent& event);

    /// @brief Restores all controls to the built-in application defaults.
    /// @param event Button event that requested the reset.
    void OnRestoreDefaults(wxCommandEvent& event);

    /// @brief Saves settings without closing the frame.
    /// @param event Apply button event.
    void OnApply(wxCommandEvent& event);

    /// @brief Saves settings and closes the frame.
    /// @param event OK button event.
    void OnOk(wxCommandEvent& event);

    /// @brief Closes the frame without saving unapplied changes.
    /// @param event Cancel button event.
    void OnCancel(wxCommandEvent& event);

    /// @brief Notifies the parent and destroys the frame.
    /// @param event Window close event.
    void OnClose(wxCloseEvent& event);

    wxListbook* _pages{};                         ///< Vertically tabbed settings container.
    wxCheckBox* _constantWindow{};                ///< Opens the Julia constant window at startup.
    wxCheckBox* _commandConsole{};                ///< Opens the command console at startup.
    wxCheckBox* _juliaMode{};                     ///< Opens Julia mode at startup.
    wxCheckBox* _colorPaletteWindow{};            ///< Opens renderer options at startup.
    wxCheckBox* _firstUse{};                      ///< Shows the welcome guide on the next launch.
    wxChoice* _theme{};                           ///< Selects the system, light, or dark application appearance.
    wxChoice* _fractalType{};                     ///< Selects the default fractal type.
    wxChoice* _colorStyle{};                      ///< Selects the default gradient color style.
    wxSpinCtrl* _maxIterations{};                 ///< Selects the default iteration limit.
    wxSpinCtrl* _paletteSize{};                   ///< Selects the default gradient palette size.
    wxSpinCtrl* _colorCycleLength{};              ///< Selects the default palette cycle length.
    wxCheckBox* _colorFractal{};                  ///< Enables coloring outside the fractal set.
    wxCheckBox* _colorSet{};                      ///< Enables coloring inside the fractal set.
    wxStaticBitmap* _gradientPreview{};           ///< Displays the current default gradient.
    wxGradient _gradient;                         ///< Gradient being edited by the frame.
    std::vector<FractalType> _fractalTypes;       ///< Values corresponding to the fractal choice entries.
    std::vector<ColorPaletteTypes> _colorStyles;  ///< Values corresponding to the color style choice entries.
    std::function<void(const AppConfig&)> _configChanged; ///< Applies saved settings to the running application.
    bool _closing{};                              ///< Prevents duplicate close notifications.
};
