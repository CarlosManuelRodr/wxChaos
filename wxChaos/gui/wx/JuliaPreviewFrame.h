/**
* @file JuliaPreviewFrame.h
* @brief Defines the wxWidgets Julia preview frame.
*/

#pragma once

#include <wx/timer.h>
#include "FractalCanvas.h"
#include "FractalToolbar.h"

wxDECLARE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

/**
* @class JuliaPreviewFrame
* @brief wxWidgets frame that previews the Julia variant of the active fractal.
*
* JuliaPreviewFrame hosts a FractalCanvas and the shared FractalToolbar so the
* Julia preview uses the same themed wxWidgets shell and interaction tools as
* the main window. It keeps renderer options and Julia constants synchronized
* with the main FractalCanvas while leaving room for a future frame menu.
*/
class JuliaPreviewFrame : public wxFrame
{
public:
    /// @brief Creates the Julia preview frame.
    /// @param parent Parent wxWidgets window.
    /// @param target Main fractal canvas used as the source of Julia constants.
    /// @param fractalType Julia fractal type to preview.
    /// @param juliaOpt Initial renderer and fractal options.
    /// @param zoomStepPercent Mouse-wheel zoom step percentage.
    /// @param zoomInertiaMilliseconds Smooth zoom inertia duration.
    /// @param size Initial preview canvas size.
    JuliaPreviewFrame(
        wxWindow* parent,
        FractalCanvas* target,
        FractalType fractalType,
        const Options& juliaOpt,
        int zoomStepPercent,
        int zoomInertiaMilliseconds,
        const wxSize& size = wxSize(640, 480));

    /// @brief Applies renderer settings from the main fractal.
    /// @param options Renderer options to apply.
    void SetRendererOptions(const Options& options) const;

    /// @brief Applies a new Julia constant.
    /// @param real Real component.
    /// @param imaginary Imaginary component.
    void SetConstant(double real, double imaginary) const;

    /// @brief Applies mouse-wheel zoom settings.
    /// @param zoomStepPercent Mouse-wheel zoom step percentage.
    /// @param zoomInertiaMilliseconds Smooth zoom inertia duration.
    void SetZoomOptions(int zoomStepPercent, int zoomInertiaMilliseconds) const;

private:
    FractalCanvas* _target{};
    FractalCanvas* _previewCanvas{};
    FractalToolbar* _toolbar{};
    wxTimer _constantSyncTimer;

    void ApplyRendererOptions(const Options& options) const;
    void SyncConstantFromTarget();
    void OnConstantSyncTimer(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);
};
