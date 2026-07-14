/**
* @file FractalToolbar.h
* @brief Defines the shared wxWidgets toolbar for fractal interactions.
*/

#pragma once

#include <functional>
#include <wx/bmpbndl.h>
#include <wx/toolbar.h>
#include "canvas/FractalInteractionTool.h"

/**
* @class FractalToolbar
* @brief Vertical toolbar for selecting fractal interaction tools and color rotation.
*
* FractalToolbar owns the wxWidgets tool buttons, theme-aware SVG icons, and
* toggle icon state. Parent windows provide callbacks to apply the selected
* interaction tool and to toggle color rotation on their active fractal canvas.
*/
class FractalToolbar : public wxToolBar
{
public:
    /// @brief Callback invoked when the active interaction tool changes.
    using ToolChangedHandler = std::function<void(FractalInteractionTool)>;

    /// @brief Callback invoked when the color rotation button is clicked.
    /// @return true when the target accepted the toggle request.
    using ColorRotationHandler = std::function<bool()>;

    /// @brief Callback invoked when the information button is clicked.
    using InformationHandler = std::function<void()>;

private:
    enum
    {
        ID_CURSOR = wxID_HIGHEST + 200,
        ID_HAND,
        ID_ZOOM,
        ID_POINT_PICKER,
        ID_INFORMATION,
        ID_COLOR_ROTATION
    };

    ToolChangedHandler _toolChangedHandler;
    ColorRotationHandler _colorRotationHandler;
    InformationHandler _informationHandler;
    bool _colorRotationActive{};

    [[nodiscard]] static wxBitmapBundle CreateInteractionToolBitmap(FractalInteractionTool tool);
    [[nodiscard]] static wxBitmapBundle CreateInformationToolBitmap();
    [[nodiscard]] static wxBitmapBundle CreateColorRotationToolBitmap(bool active);
    void UpdateColorRotationTool();
    void OnInteractionTool(wxCommandEvent& event);
    void OnInformation(wxCommandEvent& event);
    void OnColorRotation(wxCommandEvent& event);

public:
    /// @brief Creates the toolbar and all tool buttons.
    /// @param parent Parent wxWidgets window.
    explicit FractalToolbar(wxWindow* parent);

    /// @brief Sets the callback used to apply interaction tool changes.
    /// @param handler Callback receiving the newly selected tool.
    void SetToolChangedHandler(ToolChangedHandler handler);

    /// @brief Sets the callback used to toggle color rotation.
    /// @param handler Callback returning false when the toggle should be rejected.
    void SetColorRotationHandler(ColorRotationHandler handler);

    /// @brief Sets the callback used to show fractal information.
    /// @param handler Callback to invoke when the information tool is clicked.
    void SetInformationHandler(InformationHandler handler);

    /// @brief Enables or disables the information tool.
    /// @param enabled true when the active fractal has documentation.
    void SetInformationEnabled(bool enabled);

    /// @brief Updates the selected interaction tool button.
    /// @param tool Tool to show as selected.
    void SetInteractionTool(FractalInteractionTool tool);

    /// @brief Resets color rotation to the inactive play-icon state.
    void ResetColorRotationTool();

    /// @brief Enables color rotation when the active fractal supports palette animation.
    /// @param enabled true when color rotation is available.
    void SetColorRotationEnabled(bool enabled);
};
