#pragma once

#include <wx/eventfilter.h>
#include <wx/colour.h>
#include "common/AppAppearance.h"

class wxWindow;

/**
 * @class AppTheme
 * @brief Applies the selected light or dark appearance to wxChaos windows.
 *
 * The global event filters themes newly shown windows, while ApplyToAllWindows
 * updates every window that is already open when the setting changes.
 */
class AppTheme final : public wxEventFilter
{
public:
    /// @brief Installs the global window event filter.
    static void Install();

    /// @brief Removes the global window event filter.
    static void Uninstall();

    /// @brief Selects the current application appearance.
    /// @param dark Whether the dark appearance should be used.
    static void SetDark(bool dark);

    /// @brief Selects the current application appearance.
    /// @param appearance Appearance preference to resolve and apply.
    static void SetAppearance(AppAppearance appearance);

    /// @brief Resolves an appearance preference to the effective dark-mode state.
    /// @param appearance Appearance preference to resolve.
    /// @return True when the application should use dark colors.
    [[nodiscard]] static bool ResolveDark(AppAppearance appearance);

    /// @brief Reports whether the dark appearance is selected.
    /// @return True when dark appearance colors are active.
    [[nodiscard]] static bool IsDark();

    /// @brief Applies the current appearance to a window and all its children.
    /// @param window Root window to update.
    static void ApplyToWindow(wxWindow* window);

    /// @brief Applies the current appearance to every open top-level window.
    static void ApplyToAllWindows();

    /// @brief Returns the main background color for the current appearance.
    /// @return Background color used by frames and panels.
    [[nodiscard]] static wxColour Background();

    /// @brief Returns the editable-control background color.
    /// @return Background color used by text and list controls.
    [[nodiscard]] static wxColour ControlBackground();

    /// @brief Returns the primary text color for the current appearance.
    /// @return Foreground color used by ordinary controls.
    [[nodiscard]] static wxColour Foreground();

    /**
     * @brief Applies the theme when a window is about to be shown.
     * @param event Event passing through the application filter.
     * @return wxEventFilter result indicating that normal processing should continue.
     */
    int FilterEvent(wxEvent& event) override;

private:
    /// @brief Returns the singleton filter instance.
    /// @return Process-wide application theme service.
    static AppTheme& Instance();

    /// @brief Applies colors and native theme hints to one window.
    /// @param window Window to update without traversing its children.
    static void ApplyToSingleWindow(wxWindow* window);

    bool _installed{}; ///< Tracks whether the global event filter is installed.
    bool _dark{};      ///< Stores the selected dark appearance state.
    bool _applying{};  ///< Prevents native theme updates from re-entering the filter.
};
