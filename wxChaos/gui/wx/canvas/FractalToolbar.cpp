#include "canvas/FractalToolbar.h"
#include <string>
#include <utility>
#include "AppPaths.h"
#include "common/AppTheme.h"

using namespace std;

FractalToolbar::FractalToolbar(wxWindow* parent)
    : wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL | wxTB_FLAT | wxTB_NODIVIDER)
{
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
    AddRadioTool(
        ID_POINT_PICKER,
        wxEmptyString,
        CreateInteractionToolBitmap(FractalInteractionTool::PointPicker),
        wxNullBitmap,
        "Point picker",
        "Click a point to inspect the fractal result there");
    wxToolBarBase::AddSeparator();
    wxToolBarBase::AddStretchableSpace();
    AddTool(
        ID_INFORMATION,
        wxEmptyString,
        CreateInformationToolBitmap(),
        wxNullBitmap,
        wxITEM_NORMAL,
        "Information",
        "Show information about this fractal");
    AddCheckTool(
        ID_COLOR_ROTATION,
        wxEmptyString,
        CreateColorRotationToolBitmap(false),
        wxNullBitmap,
        "Color rotation",
        "Animate fractal colors");
    wxToolBar::Realize();
    SetInteractionTool(FractalInteractionTool::Cursor);
    ResetColorRotationTool();

    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_CURSOR);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_HAND);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_ZOOM);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInteractionTool, this, ID_POINT_PICKER);
    Bind(wxEVT_TOOL, &FractalToolbar::OnInformation, this, ID_INFORMATION);
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

void FractalToolbar::SetInformationHandler(InformationHandler handler)
{
    _informationHandler = std::move(handler);
}

void FractalToolbar::SetInformationEnabled(const bool enabled)
{
    EnableTool(ID_INFORMATION, enabled);
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
        case FractalInteractionTool::PointPicker:
            ToggleTool(ID_POINT_PICKER, true);
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

void FractalToolbar::SetColorRotationEnabled(const bool enabled)
{
    if (!enabled)
        _colorRotationActive = false;
    EnableTool(ID_COLOR_ROTATION, enabled);
    UpdateColorRotationTool();
}

wxSize FractalToolbar::GetToolBitmapSizeAtDefaultDpi()
{
    return {24, 24};
}

wxBitmapBundle FractalToolbar::CreateInteractionToolBitmap(const FractalInteractionTool tool)
{
    const wxSize bitmapSize = GetToolBitmapSizeAtDefaultDpi();
    const string handIcon = AppTheme::IsDark() ? "hand_dark.svg" : "hand_light.svg";
    const string zoomIcon = AppTheme::IsDark() ? "zoom_dark.svg" : "zoom_light.svg";
    const string pickerIcon = AppTheme::IsDark() ? "picker_dark.svg" : "picker_light.svg";
    const string cursorIcon = AppTheme::IsDark() ? "cursor_dark.svg" : "cursor_light.svg";

    switch (tool)
    {
        case FractalInteractionTool::Hand:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", handIcon}), bitmapSize);
        case FractalInteractionTool::Zoom:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", zoomIcon}), bitmapSize);
        case FractalInteractionTool::PointPicker:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", pickerIcon}), bitmapSize);
        case FractalInteractionTool::Cursor:
        default:
            return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", cursorIcon}), bitmapSize);
    }
}

wxBitmapBundle FractalToolbar::CreateInformationToolBitmap()
{
    const string icon = AppTheme::IsDark() ? "help_dark.svg" : "help_light.svg";
    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), GetToolBitmapSizeAtDefaultDpi());
}

wxBitmapBundle FractalToolbar::CreateColorRotationToolBitmap(const bool active)
{
    const string icon = active
        ? (AppTheme::IsDark() ? "stop_dark.svg" : "stop_light.svg")
        : (AppTheme::IsDark() ? "play_dark.svg" : "play_light.svg");

    return wxBitmapBundle::FromSVGFile(AppPaths::ResourceFile({"Icons", icon}), GetToolBitmapSizeAtDefaultDpi());
}

void FractalToolbar::UpdateColorRotationTool()
{
    ToggleTool(ID_COLOR_ROTATION, _colorRotationActive);
    SetToolNormalBitmap(ID_COLOR_ROTATION, CreateColorRotationToolBitmap(_colorRotationActive));
    Refresh();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void FractalToolbar::OnInteractionTool(wxCommandEvent& event)
{
    auto tool = FractalInteractionTool::Cursor;
    switch (event.GetId())
    {
        case ID_HAND:
            tool = FractalInteractionTool::Hand;
            break;
        case ID_ZOOM:
            tool = FractalInteractionTool::Zoom;
            break;
        case ID_POINT_PICKER:
            tool = FractalInteractionTool::PointPicker;
            break;
        case ID_CURSOR:
        default:
            tool = FractalInteractionTool::Cursor;
            break;
    }

    if (_toolChangedHandler)
        _toolChangedHandler(tool);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void FractalToolbar::OnInformation(wxCommandEvent&)
{
    if (_informationHandler)
        _informationHandler();
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
