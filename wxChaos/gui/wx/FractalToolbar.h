/**
* @file FractalToolbar.h
* @brief Defines the shared wxWidgets toolbar for fractal interactions.
*/

#pragma once

#include <functional>
#include <wx/bmpbndl.h>
#include <wx/toolbar.h>
#include "FractalInteractionTool.h"

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

    /// @brief Creates the toolbar and all tool buttons.
    /// @param parent Parent wxWidgets window.
    explicit FractalToolbar(wxWindow* parent);

    /// @brief Sets the callback used to apply interaction tool changes.
    /// @param handler Callback receiving the newly selected tool.
    void SetToolChangedHandler(ToolChangedHandler handler);

    /// @brief Sets the callback used to toggle color rotation.
    /// @param handler Callback returning false when the toggle should be rejected.
    void SetColorRotationHandler(ColorRotationHandler handler);

    /// @brief Updates the selected interaction tool button.
    /// @param tool Tool to show as selected.
    void SetInteractionTool(FractalInteractionTool tool);

    /// @brief Resets color rotation to the inactive play-icon state.
    void ResetColorRotationTool();

private:
    enum
    {
        ID_CURSOR = wxID_HIGHEST + 200,
        ID_HAND,
        ID_ZOOM,
        ID_COLOR_ROTATION
    };

    ToolChangedHandler _toolChangedHandler;
    ColorRotationHandler _colorRotationHandler;
    bool _colorRotationActive{};

    wxBitmapBundle CreateInteractionToolBitmap(FractalInteractionTool tool) const;
    wxBitmapBundle CreateColorRotationToolBitmap(bool active) const;
    void UpdateColorRotationTool();
    void OnInteractionTool(wxCommandEvent& event);
    void OnColorRotation(wxCommandEvent& event);
};
