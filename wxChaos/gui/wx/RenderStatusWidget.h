/**
* @file RenderStatusWidget.h
* @brief Defines the clickable render status control used in status bars.
*/

#pragma once

#include <functional>
#include <wx/panel.h>
#include <wx/timer.h>
#include "sfml/FractalPresenter.h"

/**
* @class RenderStatusWidget
* @brief Native wxWidgets status control for iterations and render activity.
*
* RenderStatusWidget paints a compact boxed status label for the active fractal.
* It displays the iteration count while idle and switches to an animated
* rendering indicator while the presenter is rendering. Clicking the control
* invokes a caller-provided action, usually opening IterationsDialog.
*/
class RenderStatusWidget : public wxPanel
{
public:
    /// @brief Callback invoked when the user clicks the status control.
    using ClickHandler = std::function<void()>;

    /// @brief Creates a render status widget.
    /// @param parent Parent wxWidgets window, normally a wxStatusBar.
    /// @param presenter Presenter supplying render state and iterations.
    /// @param clickHandler Callback invoked when the widget is clicked.
    RenderStatusWidget(wxWindow* parent, FractalPresenter* presenter, ClickHandler clickHandler);

    /// @brief Stops the refresh timer before the widget is destroyed.
    ~RenderStatusWidget() override;

    /// @brief Updates the presenter whose state is displayed.
    /// @param presenter Presenter supplying render state and iterations.
    void SetPresenter(FractalPresenter* presenter);

private:
    FractalPresenter* _presenter{};
    ClickHandler _clickHandler;
    wxTimer _timer;
    unsigned int _spinnerFrame{};

    void OnPaintEvent(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnLeftUp(wxMouseEvent& event);
};
