#include "FractalToolbar.h"
#include <string>
#include <utility>
#include "AppPaths.h"
#include "AppTheme.h"

using namespace std;

FractalToolbar::FractalToolbar(wxWindow* parent)
    : wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL | wxTB_FLAT | wxTB_NODIVIDER)
{
    SetToolBitmapSize(wxSize(48, 48));
    AddRadioTool(
        ID_CURSOR,
        wxEmptyString,
        CreateInteractionToolBitmap(FractalInteractionTool::Cursor),
        wxNullBitmap,
        "Cursor",
        "Use the default fractal interactions");
    AddRadioTool(
        ID_HAND,
        wxEmptyString,
        CreateInteractionToolBitmap(FractalInteractionTool::Hand),
        wxNullBitmap,
        "Pan",
        "Pan the fractal by dragging");
    AddRadioTool(
        ID_ZOOM,
        wxEmptyString,
        CreateInteractionToolBitmap(FractalInteractionTool::Zoom),
        wxNullBitmap,
        "Zoom",
        "Drag upward to zoom in or downward to zoom out");
    AddSeparator();
    AddStretchableSpace();
    AddCheckTool(
        ID_COLOR_ROTATION,
        wxEmptyString,
        CreateColorRotationToolBitmap(false),
        wxNullBitmap,
        "Color rotation",
        "Animate fractal colors");
    Realize();
    SetInteractionTool(FractalInteractionTool::Cursor);
    ResetColorRotationTool();

    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_CURSOR);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_HAND);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_ZOOM);
    Bind(wxEVT_TOOL, &FractalToolbar::OnColorRotation, this, ID_COLOR_ROTATION);
}

void FractalToolbar::SetToolChangedHandler(ToolChangedHandler handler)
{
    _toolChangedHandler = std::move(handler);
}

void FractalToolbar::SetColorRotationHandler(ColorRotationHandler handler)
{
    _colorRotationHandler = std::move(handler);
}

void FractalToolbar::SetInteractionTool(const FractalInteractionTool tool)
{
    switch (tool)
    {
        case FractalInteractionTool::Hand:
            ToggleTool(ID_HAND, true);
            break;
        case FractalInteractionTool::Zoom:
            ToggleTool(ID_ZOOM, true);
            break;
        case FractalInteractionTool::Cursor:
        default:
            ToggleTool(ID_CURSOR, true);
            break;
    }
}

void FractalToolbar::ResetColorRotationTool()
{
    _colorRotationActive = false;
    UpdateColorRotationTool();
}

wxBitmapBundle FractalToolbar::CreateInteractionToolBitmap(const FractalInteractionTool tool) const
{
    const string handIcon = AppTheme::IsDark() ? "hand_dark.svg" : "hand_light.svg";
    const string zoomIcon = AppTheme::IsDark() ? "zoom_dark.svg" : "zoom_light.svg";
    const string cursorIcon = AppTheme::IsDark() ? "cursor_dark.svg" : "cursor_light.svg";

    switch (tool)
    {
        case FractalInteractionTool::Hand:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", handIcon}), wxSize(48, 48));
        case FractalInteractionTool::Zoom:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", zoomIcon}), wxSize(48, 48));
        case FractalInteractionTool::Cursor:
        default:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", cursorIcon}), wxSize(48, 48));
    }
}

wxBitmapBundle FractalToolbar::CreateColorRotationToolBitmap(const bool active) const
{
    const string icon = active
        ? (AppTheme::IsDark() ? "stop_dark.svg" : "stop_light.svg")
        : (AppTheme::IsDark() ? "play_dark.svg" : "play_light.svg");

    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), wxSize(48, 48));
}

void FractalToolbar::UpdateColorRotationTool()
{
    ToggleTool(ID_COLOR_ROTATION, _colorRotationActive);
    SetToolNormalBitmap(ID_COLOR_ROTATION, CreateColorRotationToolBitmap(_colorRotationActive));
    Refresh();
}

void FractalToolbar::OnInteractionTool(wxCommandEvent& event)
{
    FractalInteractionTool tool = FractalInteractionTool::Cursor;
    switch (event.GetId())
    {
        case ID_HAND:
            tool = FractalInteractionTool::Hand;
            break;
        case ID_ZOOM:
            tool = FractalInteractionTool::Zoom;
            break;
        case ID_CURSOR:
        default:
            tool = FractalInteractionTool::Cursor;
            break;
    }

    if (_toolChangedHandler)
        _toolChangedHandler(tool);
}

void FractalToolbar::OnColorRotation(wxCommandEvent&)
{
    if (_colorRotationHandler && !_colorRotationHandler())
    {
        UpdateColorRotationTool();
        return;
    }

    _colorRotationActive = !_colorRotationActive;
    UpdateColorRotationTool();
}
