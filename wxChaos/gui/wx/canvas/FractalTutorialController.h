#pragma once

#include <functional>
#include <memory>
#include <wx/event.h>
#include <wx/timer.h>
#include "canvas/FractalInteractionTool.h"
#include "canvas/FractalTutorialModel.h"
#include "config/TutorialStatus.h"

class FractalCanvas;
class FractalToolbar;
class FractalTutorialOverlay;
class wxWindow;

/**
* @class FractalTutorialController
* @brief Coordinates tutorial progression, presentation, persistence, and feedback.
*/
class FractalTutorialController : public wxEvtHandler
{
    enum class Phase
    {
        Inactive,
        WaitingForRender,
        Active,
        SuccessTransition,
        FinalMessage,
        SkippedMessage
    };

    FractalCanvas* _canvas;
    FractalToolbar* _toolbar;
    std::unique_ptr<FractalTutorialOverlay> _overlay;
    FractalTutorialModel _model;
    std::function<void(TutorialStatus)> _statusChanged;
    wxTimer _timer;
    wxWindow* _topLevelWindow{};
    Phase _phase{Phase::Inactive};
    FractalInteractionTool _currentTool{FractalInteractionTool::Cursor};
    bool _automaticRun{};
    int _phaseElapsedMilliseconds{};

    void PersistStatus(TutorialStatus status) const;
    void PlaySuccessSound() const;
    void BeginSuccessTransition();
    void OnTimer(wxTimerEvent& event);
    void OnCanvasSize(wxSizeEvent& event);
    void OnFrameMove(wxMoveEvent& event);

public:
    FractalTutorialController(FractalCanvas* canvas, FractalToolbar* toolbar,
                              std::function<void(TutorialStatus)> statusChanged);
    ~FractalTutorialController() override;

    void Start(bool automaticRun);
    void HandleAction(FractalTutorialAction action);
    void HandleToolSelected(FractalInteractionTool tool);
    bool HandleEscape();
    [[nodiscard]] bool IsActive() const;
};
