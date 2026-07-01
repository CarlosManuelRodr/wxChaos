/**
* @file JuliaPreviewFrame.h
* @brief Defines the wxWidgets Julia preview frame.
*/

#pragma once

#include <wx/timer.h>
#include "canvas/FractalCanvas.h"
#include "canvas/FractalToolbar.h"
#include "fractal/IterationsDialog.h"
#include "canvas/RenderStatusWidget.h"

wxDECLARE_EVENT(wxEVT_JULIA_MODE_CLOSED, wxCommandEvent);

/**
* @enum JuliaPreviewFrameIds
* @brief Menu command identifiers owned by JuliaPreviewFrame.
*/
enum JuliaPreviewFrameIds
{
    ID_JULIA_PREVIEW_OPEN_IN_MAIN = wxID_HIGHEST + 250,
    ID_JULIA_PREVIEW_VIEW_TOOLBAR,
    ID_JULIA_PREVIEW_ABORT_RENDER,
    ID_JULIA_PREVIEW_REDRAW,
    ID_JULIA_PREVIEW_RESET,
    ID_JULIA_PREVIEW_SET_ITERATIONS,
    ID_JULIA_PREVIEW_INCREASE_ITERATIONS,
    ID_JULIA_PREVIEW_DECREASE_ITERATIONS,
    ID_JULIA_PREVIEW_AUTOMATIC_ITERATIONS
};

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
    FractalCanvas* _target{};
    FractalCanvas* _previewCanvas{};
    FractalToolbar* _toolbar{};
    wxMenuBar* _menubar{};
    wxMenu* _fileMenu{};
    wxMenu* _viewMenu{};
    wxMenu* _fractalMenu{};
    wxMenu* _iterationsMenu{};
    wxMenuItem* _toolbarItem{};
    wxMenuItem* _abortRenderItem{};
    wxMenuItem* _automaticIterationsItem{};
    wxStatusBar* _statusBar{};
    RenderStatusWidget* _renderStatusWidget{};
    IterationsDialog* _iterationsDialog{};
    wxTimer _constantSyncTimer;
    bool _iterationsDialogIsActive{};

    void ApplyRendererOptions(const Options& options) const;
    void CreateMenuBar();
    void CreateStatusBarControls();
    void LayoutStatusBarControls() const;
    void OpenIterationsDialog();
    void OpenFractalInformation();
    void UpdateInformationTool() const;
    void ResetColorRotationTool() const;
    void SyncConstantFromTarget() const;
    void OnConstantSyncTimer(wxTimerEvent& event);
    void OnCanvasStatusText(wxCommandEvent& event);

    /// @brief Relayouts the frame contents and status bar controls after resizing.
    /// @param event Size event emitted by wxWidgets.
    void OnResize(wxSizeEvent& event);

    /// @brief Closes the Julia preview frame.
    /// @param event Menu event emitted by wxWidgets.
    void OnCloseMenu(wxCommandEvent& event);

    /// @brief Shows or hides the Julia preview interaction toolbar.
    /// @param event Menu event emitted by wxWidgets.
    void OnToolbarVisibility(wxCommandEvent& event);

    /// @brief Opens the previewed fractal in the main window and closes this frame.
    /// @param event Menu event emitted by wxWidgets.
    void OnOpenInMainWindow(wxCommandEvent& event);

    /// @brief Aborts the current preview render.
    /// @param event Menu event emitted by wxWidgets.
    void OnAbortRender(wxCommandEvent& event);

    /// @brief Redraws the Julia preview.
    /// @param event Menu event emitted by wxWidgets.
    void OnRedraw(wxCommandEvent& event);

    /// @brief Resets the Julia preview viewport.
    /// @param event Menu event emitted by wxWidgets.
    void OnReset(wxCommandEvent& event);

    /// @brief Opens the iterations dialog for the Julia preview.
    /// @param event Menu event emitted by wxWidgets.
    void OnSetIterations(wxCommandEvent& event);

    /// @brief Increases the Julia preview iteration count.
    /// @param event Menu event emitted by wxWidgets.
    void OnIncreaseIterations(wxCommandEvent& event);

    /// @brief Decreases the Julia preview iteration count.
    /// @param event Menu event emitted by wxWidgets.
    void OnDecreaseIterations(wxCommandEvent& event);

    /// @brief Toggles automatic iterations for the Julia preview.
    /// @param event Menu event emitted by wxWidgets.
    void OnAutomaticIterations(wxCommandEvent& event);

    /// @brief Updates the abort-render menu item.
    /// @param event Update UI event emitted by wxWidgets.
    void OnUpdateAbortRender(wxUpdateUIEvent& event);

    /// @brief Updates the automatic-iterations menu item.
    /// @param event Update UI event emitted by wxWidgets.
    void OnUpdateAutomaticIterations(wxUpdateUIEvent& event);
    void OnClose(wxCloseEvent& event);

public:
    /// @brief Creates the Julia preview frame.
    /// @param parent Parent wxWidgets window.
    /// @param target Main fractal canvas used as the source of Julia constants.
    /// @param fractalType Julia fractal type to preview.
    /// @param juliaOpt Initial renderer and fractal options.
    /// @param zoomStepPercent Mouse-wheel zoom step percentage.
    /// @param zoomInertiaMilliseconds Smooth zoom inertia duration.
    /// @param size Initial preview canvas size.
    JuliaPreviewFrame(wxWindow* parent, FractalCanvas* target, FractalType fractalType, const Options& juliaOpt,
                      int zoomStepPercent, int zoomInertiaMilliseconds, const wxSize& size = wxSize(640, 480));

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
};
